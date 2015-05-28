#include <cstdio>
#include <qcsv.h>
#include <dbms/task/newtab.h>
#include <dbms/task/deltab.h>
#include <dbms/task/insert.h>
#include <dbms/task/newidx.h>
#include <dbms/task/query.h>
#include <dbms/task/occ.h>
#include <dbms/cols.h>
#include "plk.h"
#include "bed.h"
#include <typedefs.h>
#include <qcsv.h>
#include <fstream>
#include <misc.h>

using namespace std;
using namespace db::tsk;
using namespace dat::plk;

#ifndef PLINK_FAM_DEBUG
#define PLINK_FAM_DEBUG 0
#endif

#ifndef PLINK_BIM_DEBUG
#define PLINK_BIM_DEBUG 0
#endif

#ifndef PLINK_BED_DEBUG
#define PLINK_BED_DEBUG 0
#endif

class Fam :public QCsv
{
private:
	db::tsk::InsRow m_insert;
public:
	Fam(FILE* file) :QCsv(file, QCsv::SEP(' '))
	{
		db::tsk::NewTab newtab;
		newtab.tab(MDB"."FAM);	// mem table
		newtab.ovr(true);
		newtab.col<int>(FID);
		newtab.col<string>(IDV);
		newtab.col<string>(PID);
		newtab.col<string>(MID);
		newtab.col<int>(SEX);
		newtab.col<int>(PHE);
		newtab.exec();
		newtab.key(FID, IDV);

		newtab.tab(HDB"."FAM);	// hdd table, create on need
		newtab.ovr(false);
		newtab.exec();

		// if HDD table pre-exists, this will load it into MEM.
		QTsk("INSERT OR IGNORE INTO "MDB"."FAM" SELECT * FROM "HDB"."FAM);

#if (PLINK_FAM_DEBUG > 0)
		ofstream ofs(path + ".tab");
		QQry("SELECT * FROM "MDB"."FAM).print(ofs, true);
		ofs.close();
#endif
	}
protected:
	void on_init() override
	{
		m_insert.tab(MDB"."FAM);
		m_insert.col(FID, IDV, PID, MID, SEX, PHE);
		m_insert.exec();
	}
	void on_word() override
	{
		switch (icol())
		{
		case 0:	// FID
		case 4:	// SEX
		case 5:	// PHE
			m_insert.bind((int)icol(), atoi(wptr()));
			break;
		case 1:	// IDV
		case 2:	// PID
		case 3:	// MID
			m_insert.bind((int)icol(), wptr());
			break;
		default:
			break;
		}
#if (defined(PLINK_FAM_DEBUG) && PLINK_FAM_DEBUG>0)
		printf("%s", wptr());
		if (icol() < ncol() - 1)
			printf("\t");
		else
			printf("\n");
#endif
	}
	void on_line() override
	{
		m_insert.next();
	}
	void on_fini() override
	{
		QTsk("INSERT OR IGNORE INTO "HDB"."FAM" SELECT * FROM "MDB"."FAM);
	}
};

class Bim :public QCsv
{
private:
	db::tsk::InsRow m_insert;
public:
	Bim(FILE* file) :QCsv(file, QCsv::SEP('\t'))
	{
		db::tsk::NewTab newtab;
		newtab.tab(MDB"."BIM);	// mem table
		newtab.col<int>(CHR);
		newtab.col<string>(VID);
		newtab.col<uint>(CMP);
		newtab.col<uint>(BSP);
		newtab.col<string>(AL1);
		newtab.col<string>(AL2);
		newtab.key(CHR, VID);
		newtab.exec();

		newtab.tab(HDB"."BIM);	// harddisk mirror, create on need
		newtab.ovr(false);		// do not overwrite existing table
		newtab.exec();

		// if HDD table pre-exists, this will load it into MEM.
		QTsk("INSERT OR IGNORE INTO "MDB"."BIM" SELECT * FROM "HDB"."BIM);

#if (PLINK_BIM_DEBUG > 0)
		ofstream ofs(path + ".tab");
		this->chr(3);
		this->bp1(10000000);
		this->bp2(11000000);
		this->print(ofs, true);
		ofs.close();
#endif
	}

	void on_init()
	{
		m_insert.tab(MDB"."BIM);
		m_insert.col(CHR, VID, CMP, BSP, AL1, AL2);
		m_insert.exec();
	}
	void on_word()
	{
		switch (icol())
		{
		case 0:		// CHR
			m_insert.bind(0, atoi(wptr()));
			break;
		case 1:		// VID
			m_insert.bind(1, wptr());
			break;
		case 2:		// CMP
			m_insert.bind(2, atoi(wptr()));
			break;
		case 3:		// BSP
			m_insert.bind(3, atoi(wptr()));
			break;
		case 4:		// AL1
			m_insert.bind(4, wptr());
			break;
		case 5:		// AL2
			m_insert.bind(5, wptr());
			break;
		default:	// skip 2.CMP
			break;
		}
#if (defined(PLINK_BIM_DEBUG) && PLINK_BIM_DEBUG>0)
		//printf("%s", wptr());
		//if (icol() < ncol() - 1)
		//	printf("\t");
		//else
		//	printf("\n");
#endif
	}
	void on_line()
	{
		m_insert.next();
#if (defined(PLINK_BIM_DEBUG) && PLINK_BIM_DEBUG>0)
		if ((irow() & 0x0FFF))
			return;
		else
			printf("Reading BIM line: %8d/%8d\n", irow(), nrow());
#endif
	}
	void on_fini()
	{
		// update HDD table
		QTsk("INSERT OR IGNORE INTO "HDB"."BIM" SELECT * FROM "MDB"."BIM);
	}
};

Bed::Bed(ifstream ifs, sztp nidv, sztp nloc) :m_nidv(nidv), m_nloc(nloc)
{
	if (ifs.get() != MAG1 || ifs.get() != MAG2)
		GEXP(exp_bed() << errm("Corrupt plink BED head, incorrect magic byte."));
	m_mode = ifs.get();
	sztp r, c;	// row, column count.
	if (m_mode == Bed::VRMJ)	// the 3rd byte: SNP or Individual major indicator.
	{
		r = m_nloc;
		c = m_nidv;	// list all individual for the i th. SNP (0 < i < nloc == c)
	}
	else
	{
		r = m_nidv;
		c = m_nloc;	// list all SNP for the i th. individual (0 < i < nidv == c)
	}

	m_rlen = c >> 2;		// row mem = gno col / 4	(1 byte encode 4 genotype, 2 bits each)
	if (c & 0x03)		// last few genotype of a row round up one more byte.
		m_rlen++;
	m_rlg2 = 0;
	for (m_rlg2 = 0; 1ULL << m_rlg2 < m_rlen; m_rlg2++);
	m_size = r << m_rlg2;	// total memory = row memory * number of rows.
	m_pmem = boost::shared_array<char>(new char[m_size]);
	for (sztp i = 0; i < r; i++)
	{
		ifs.read(m_pmem.get() + (i << m_rlg2), m_rlen);
	}
	if (ifs.bad())
		GEXP(exp_bed() << errs("Exception while reading Plink BED."));
	if (ifs.get() != EOF)
		GEXP(exp_bed() << errs("Plink BED is longer than expected."));
}
byte Bed::gno(sztp i_idv, sztp j_loc)	// get i th. individul's j th. loci genotype, in BED coding.
{
	byte mask[4] = { 0x03, 0x0C, 0x30, 0xC0 };
	char shft[4] = { 0, 2, 4, 6 };
	char malc[4] = { 2, 3, 1, 0 };	// BED encode --> Minor allel count. (3 = missing)
	byte g4;	// four genotype encoded in one byte
	char pair;
	if (m_mode == Bed::VRMJ)
	{
		pair = i_idv & 0x03;
		g4 = m_pmem.get()[j_loc << m_rlg2 | i_idv >> 2];
	}
	else
	{
		pair = j_loc & 0x03;
		g4 = m_pmem.get()[i_idv << m_rlg2 | j_loc >> 2];
	}
	g4 &= mask[pair];
	g4 >>= shft[pair];
	return malc[g4];				// BED encode --> Minor allel count. (3 = missing)
}
void Bed::toPed(std::istream& rng)
{
	char row[1024];							// range record row
	while (!rng.getline(row, 1024).eof())	// browse range list (no header line!!!)
	{
		stringstream ssRow(row);
		string seq;
		int chr;
		int bp1;
		int bp2;
		ssRow >> seq;
		ssRow >> chr;
		ssRow >> bp1;
		ssRow >> bp2;

		// prepare output streams
		ofstream ofsBed = ofstream(seq + ".bed");
		ofstream ofsBim = ofstream(seq + ".bim");
		ofstream ofsFam = ofstream(seq + ".bim");

		std::ostringstream ssqBim;
		ssqBim << "SELECT "RID", "VID", "CHR", "BSP" FROM "BIM" WHERE "CHR" = " << chr << " AND " << bp1 << " <= "BSP" AND "BSP" <= " << bp2;
		QQry qryMap(ssqBim.str(), false);	// query of genome map

		// prepare individual ID list.
		std::ostringstream ssqFam;
		ssqFam << "SELECT "FID", "IDV", "PID", "MID", "SEX", "PHE" FROM "FAM;
		QQry qryIdv(ssqFam.str(), false);		// query of individual
		int nidv = qryIdv.print(ofsFam, false);	// print

		// in range (chr, bp1, bp2)
		sztp gv0 = 0;	// 0 based indexe of genome variant 0 (the first)
		sztp gvN = 0;	// 0 based indexe of genome variant N (behind the last)
		int nGV = 0;	// number of genome variant
		while (qryMap.more())
		{
			// output genotype file header
			ofsBed << " ";						// the first delimiter leave top-left cell empty in *.raw genotype file.
			gvN = qryMap.get<sztp>(0);
			nGV++;
			ofsBed << qryMap.get<string>(1);	// header line - genome variants (gvr)

			// output map file
			ofsBim << qryMap.get<string>(1) << "\t";	// genome variant id
			ofsBim << qryMap.get<sztp>(2) << "\t";		// chromosome
			ofsBim << qryMap.get<sztp>(3) << "\n";		// location on chromosome
		}
		ofsBed << "\n";	// ready for genotype output after header line
		gv0 = gvN - nGV;

		for (sztp i = 0; i < m_nidv; i++)			// output the i th. row of RAW file.
		{
			for (sztp j = gv0; j < gvN; j++)
				ofsBed << " " << (char)(gno(i, j) + '0');	// output the i th. idv's j th. dosage genotype
			ofsBed << "\n";
		}

		ofsBim.close();
		ofsBed.close();
	}
}
void Bed::toRaw(std::istream& rng, std::string out, int bdr)
{
	// prepare individual ID list.
	std::ostringstream ssqIdv;
	ssqIdv << "SELECT "IDV" FROM "FAM;
	QQry qryIdv(ssqIdv.str(), false);	// query of individual
	SVec vecIID;
	while (qryIdv.more())
		vecIID.push_back(qryIdv.get<string>(0));
	if (vecIID.empty())
		GEXP(exp_bed() << errs("Plink BED file set contains 0 individual."));

	while (!rng.eof())	// browse range list (no header line!!!)
	{
		string row;
		std::getline(rng, row);
		if (row.empty())
			continue;	// skip empty range record.
		stringstream ssRow(row);
		string seq;
		int chr;
		int bp1;
		int bp2;
		ssRow >> seq;
		ssRow >> chr;
		ssRow >> bp1;
		ssRow >> bp2;
		bp1 -= bdr;
		bp2 += bdr;

		// check number of genome variants
		std::ostringstream ssqNGV;
		ssqNGV << "SELECT COUNT(*), MIN("RID"), MAX("RID") FROM "BIM" WHERE "CHR" = " << chr << " AND " << bp1 << " <= "BSP" AND "BSP" <= " << bp2;
		QQry qryNGV(ssqNGV.str());
		uint nGV = qryNGV.get<uint>(0);		// variant count
		if (nGV < 1)
			continue;	// skip due to empty variant list
		uint gv0 = qryNGV.get<uint>(1) - 1;	// zero based index of first variant	(RID is one based)
		uint gvN = qryNGV.get<uint>(2);		// zero based index after last variant	(RID is one based)

		// prepare output streams
		ofstream ofsGno = ofstream(out + "/" + seq + ".dsg");
		ofstream ofsMap = ofstream(out + "/" + seq + ".pos");

		// extract variants in range (chr, bp1, bp2)
		std::ostringstream ssqMap;
		ssqMap << "SELECT "VID", "CHR", "BSP" FROM "BIM" WHERE "CHR" = " << chr << " AND " << bp1 << " <= "BSP" AND "BSP" <= " << bp2;
		QQry qryMap(ssqMap.str(), false);	// query of genome map
		while (qryMap.more())
		{
			// output genotype file header
			ofsGno << " ";						// the first delimiter leave top-left cell empty in *.raw genotype file.
			ofsGno << qryMap.get<string>(0);	// header line - genome variants (gvr)

			// output map file
			ofsMap << qryMap.get<string>(0) << "\t";	// genome variant id
			ofsMap << qryMap.get<uint>(1) << "\t";		// chromosome
			ofsMap << qryMap.get<uint>(2) << "\n";		// base pair location on chromosome
		}
		ofsGno << "\n";	// ready for genotype output after header line

		uint nidv = (uint)vecIID.size();
		for (uint i = 0; i < nidv; i++)				// output the i th. row of RAW file.
		{
			ofsGno << vecIID[i];					// output the i th. idv ID
			for (uint j = gv0; j < gvN; j++)
				ofsGno << " " << (char)(gno(i, j) + '0');	// output the i th. idv's j th. dosage genotype
			ofsGno << "\n";
		}
		ofsMap.close();
		ofsGno.close();
	}
}
void Bed::toRaw(std::ostream& out, int chr, sztp bp1, sztp bp2, bool hdr)
{
	std::ostringstream sqlSS;
	sqlSS << "SELECT "RID", "VID" FROM "BIM" WHERE "CHR" = " << chr << " AND " << bp1 - 1 << " < "BSP" AND "BSP" < " << bp2;
	QQry qry(sqlSS.str(), false);

	sztp gvr0 = 0;	// indexing genomic variable 0 (the first
	sztp gvrN = 0;	// indexing genomic variable N (behind the last)
	if (qry.more())
	{
		gvr0 = qry.get<sztp>(0) - 1;
		gvrN = gvr0 + 1;
		if (hdr)
			out << qry.get<string>(1);
	}
	if (gvr0 < gvrN)
	{
		while (qry.more())
		{
			gvrN++;
			if (hdr)
				out << " " << qry.get<string>(1);
		}
		if (hdr)
			out << "\n";

		for (sztp i = 0; i < m_nidv; i++)
		{
			if (gvr0 < gvrN)
				out << (char)(gno(i, gvr0) + '0');
			for (sztp j = gvr0 + 1; j < gvrN; j++)
				out << " " << (char)(gno(i, j) + '0');
			out << "\n";
		}
	}
}
void Bed::ptRng(std::istream& rng, std::ostream& out, int bdr)
{
	// prepare individual ID list.
	std::ostringstream ssqIdv;
	ssqIdv << "SELECT "IDV" FROM "FAM;
	QQry qryIdv(ssqIdv.str(), false);	// query of individual
	SVec vecIID;
	while (qryIdv.more())
		vecIID.push_back(qryIdv.get<string>(0));
	if (vecIID.empty())
		GEXP(exp_bed() << errs("Plink BED file set contains 0 individual."));

	out << "seq\tchr\tbp1\tbp2\tngv\trmk\n";	// the output header line.
	while (!rng.eof())	// browse range list (input has no header line!!!)
	{
		string row;
		std::getline(rng, row);
		if (row.empty())
			continue;	// skip empty range record.
		stringstream ssRow(row);
		string seq;
		string rmk;
		int chr;
		int bp1;
		int bp2;
		ssRow >> seq;
		ssRow >> chr;
		ssRow >> bp1;
		ssRow >> bp2;
		std::getline(ssRow, rmk);
		bp1 -= bdr;
		bp2 += bdr;

		// check number of genome variants
		std::ostringstream ssqNGV;
		ssqNGV << "SELECT COUNT(*) FROM "BIM" WHERE "CHR" = " << chr << " AND " << bp1 << " <= "BSP" AND "BSP" <= " << bp2;
		QQry qryNGV(ssqNGV.str());
		uint ngv = qryNGV.get<uint>(0);		// variant count
		out << seq << "\t" << chr << "\t" << bp1 << "\t" << bp2 << "\t" << ngv << rmk << "\n";
	}
}
Bed dat::plk::parse_bed(string path)
{
	FILE* file = 0;
	file = misc::open(path + ".fam");
	Fam fam(file);
	sztp nidv = QQry("SELECT COUNT(*) FROM "MDB"."FAM).get<sztp>(0);
	if (nidv == 0)	// Parse and build FAM table is needed
	{
		fam.scan();
		fam.proc();
		nidv = QQry("SELECT COUNT(*) FROM "MDB"."FAM).get<sztp>(0);
	}
	fclose(file);

	file = misc::open(path + ".bim");
	Bim bim(file);
	sztp nloc = QQry("SELECT COUNT(*) FROM "MDB"."BIM).get<sztp>(0);
	if (nloc == 0)	// Parse and build BIM table is needed
	{
		bim.scan();
		bim.proc();
		nloc = QQry("SELECT COUNT(*) FROM "MDB"."BIM).get<sztp>(0);
	}
	fclose(file);

	string fbed = path + ".bed";
	Bed gno(fbed, nidv, nloc);
#if (PLINK_BED_DEBUG > 0)
	std::ofstream ofs(path + "sub.raw");
	gno.toRaw(ofs, 5, 57123456, 60345678, false);
#endif
	return gno;
}