#ifndef _GWAS_PLINK_BED_H_
#define _GWAS_PLINK_BED_H_

#include <string>
#include <iostream>
#include <fstream>
#include <typedefs.h>
#include <boost/smart_ptr.hpp>

#define FAM "fam"
#define BIM "bim"
#define BED "bed"

#define CBIG "chr_bsp_idv_gno"
#define C___ 0xFF00000000000000	// << 56
#define _B__ 0x00FFFFFFFF000000 // << 24
#define __I_ 0x0000000000FFFF00 // << 8
#define ___G 0x00000000000000FF // << 0

#define CBIG_MAKE(c,b,i,g) (c<<56 | b<<24 | i<<8 | g<<0)
#define CBIG_C(x) ((x & C___) >> 56)
#define CBIG_B(x) ((x & _B__) >> 24)
#define CBIG_I(x) ((x & __I_) >> 8)
#define CBIG_G(x) ((x & ___G) >> 0)

#define ICBG "idv_chr_bsp_gno"
#define I___ 0xFFFF000000000000 // << 48
#define _C__ 0x0000FF0000000000 // << 40
#define __B_ 0x000000FFFFFFFF00 // << 8
#define ___G 0x00000000000000FF // << 0
#define ICBG_MAKE(i,c,b,g) (i<<48 | c<<40 | b<<8 | g<<0)

#define ICBG_I(x) ((x & I___) >> 48)
#define ICBG_C(x) ((x & _C__) >> 40)
#define ICBG_B(x) ((x & __B_) >> 8)
#define ICBG_G(x) ((x & ___G) >> 0)

#define CBIG_ICBG(x) ((C___&x) >> 16 | (_B__&x) >> 16 | (__I_&x) << 40 | (___G&x))
#define ICBG_CBIG(x) ((_C__&x) << 16 | (__B_&x) << 16 | (__I_&x) >> 40 | (___G&x))

namespace dat
{
	namespace plk
	{
		class Bed
		{
		private:
			static const byte MAG1 = 0x6C;
			static const byte MAG2 = 0x1B;
			static const byte VRMJ = 0x01;	// variant major mode
			static const byte IDMJ = 0x00;	// Individual major mode.
			sztp	m_nidv;
			sztp	m_nloc;
			sztp	m_ngno;
			byte	m_mode;
			sztp	m_size = 0;	// total size in bytes
			sztp	m_rlen = 0;	// row length in bytes.
			byte	m_rlg2 = 0;	// log(2, rlen), round up to integer
			boost::shared_array<char> m_pmem;

		public:
			Bed(std::ifstream ifs, sztp nidv, sztp nloc);
			Bed(std::string path, sztp nidv, sztp nloc) :
				Bed(std::ifstream(path, std::ios_base::in | std::ios_base::binary),
				nidv, nloc)
			{}
			Bed()
			{}
			virtual ~Bed()	{}

			/* ---------- Field Accessors ----------*/
			inline sztp nidv() const
			{
				return m_nidv;
			}
			inline sztp nidv(sztp nidv)
			{
				return m_nidv = nidv;
			}
			inline byte mode() const
			{
				return m_mode;
			}
			inline sztp size() const
			{
				return m_size;
			}
			inline char* pmem() const
			{
				return m_pmem.get();
			}
			inline sztp ngno() const
			{
				return m_ngno;
			}
			/* ---------- Data Accessor & Converters ----------*/
			byte gno(sztp i_idv, sztp j_loc);	// get genotype at i th. individul's j th. loci, coded in minor allel count.
			void toRaw(std::istream& rng, std::string dir = "", int bdr=0);
			void toRaw(std::ostream& ofs, int chr, sztp bp1, sztp bp2, bool hdr = true);
			void toPed(std::istream& rng);
			void ptRng(std::istream& rng, std::ostream& out, int bdr);
		};
		Bed parse_bed(std::string path);
		class exp_bed : public exp_plk	{};
	}
}

#endif