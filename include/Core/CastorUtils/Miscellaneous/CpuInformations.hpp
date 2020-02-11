/*
See LICENSE file in root folder
*/
#ifndef ___CU_CpuInformations_H___
#define ___CU_CpuInformations_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include <bitset>

namespace castor
{
	/**
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		09/02/2016
	\~english
	\brief		Retrieves the basic CPU informations.
	\~french
	\brief		Récupère les informations sur le CPU.
	*/
	class CpuInformations
	{
	private:
		struct CpuInformationsInternal
		{
			CpuInformationsInternal();

			bool m_isIntel{ false };
			bool m_isAMD{ false };
			uint32_t m_coreCount{ 0u };
			std::string m_vendor{};
			std::bitset< 32 > m_f_1_ECX{ 0 };
			std::bitset< 32 > m_f_1_EDX{ 0 };
			std::bitset< 32 > m_f_7_EBX{ 0 };
			std::bitset< 32 > m_f_7_ECX{ 0 };
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API CpuInformations();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~CpuInformations();
		/**
		 *\~english
		 *\return		The number of cores (Physical + Virtual).
		 *\~french
		 *\return		Le nombre de coeurs (Physiques + Virtuels).
		 */
		inline uint32_t getCoreCount()const
		{
			return m_internal.m_coreCount;
		}
		/**
		 *\~english
		 *\return		The CPU vendor.
		 *\~french
		 *\return		Le vendeur du CPU.
		 */
		inline std::string getVendor()const
		{
			return m_internal.m_vendor;
		}
		/**
		 *\~english
		 *\return		The SSE3 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE3.
		 */
		inline bool SSE3()const
		{
			return m_internal.m_f_1_ECX[0];
		}
		/**
		 *\~english
		 *\return		The PCLMULQDQ instructions support.
		 *\~french
		 *\return		Le support des instructions PCLMULQDQ.
		 */
		inline bool PCLMULQDQ()const
		{
			return m_internal.m_f_1_ECX[1];
		}
		/**
		 *\~english
		 *\return		The MONITOR instructions support.
		 *\~french
		 *\return		Le support des instructions MONITOR.
		 */
		inline bool MONITOR()const
		{
			return m_internal.m_f_1_ECX[3];
		}
		/**
		 *\~english
		 *\return		The SSSE3 instructions support.
		 *\~french
		 *\return		Le support des instructions SSSE3.
		 */
		inline bool SSSE3()const
		{
			return m_internal.m_f_1_ECX[9];
		}
		/**
		 *\~english
		 *\return		The FMA instructions support.
		 *\~french
		 *\return		Le support des instructions FMA.
		 */
		inline bool FMA()const
		{
			return m_internal.m_f_1_ECX[12];
		}
		/**
		 *\~english
		 *\return		The CMPXCHG16B instructions support.
		 *\~french
		 *\return		Le support des instructions CMPXCHG16B.
		 */
		inline bool CMPXCHG16B()const
		{
			return m_internal.m_f_1_ECX[13];
		}
		/**
		 *\~english
		 *\return		The SSE4.1 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE4.1.
		 */
		inline bool SSE41()const
		{
			return m_internal.m_f_1_ECX[19];
		}
		/**
		 *\~english
		 *\return		The SSE4.2 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE4.2.
		 */
		inline bool SSE42()const
		{
			return m_internal.m_f_1_ECX[20];
		}
		/**
		 *\~english
		 *\return		The MOVBE instructions support.
		 *\~french
		 *\return		Le support des instructions MOVBE.
		 */
		inline bool MOVBE()const
		{
			return m_internal.m_f_1_ECX[22];
		}
		/**
		 *\~english
		 *\return		The POPCNT instructions support.
		 *\~french
		 *\return		Le support des instructions POPCNT.
		 */
		inline bool POPCNT()const
		{
			return m_internal.m_f_1_ECX[23];
		}
		/**
		 *\~english
		 *\return		The AES instructions support.
		 *\~french
		 *\return		Le support des instructions AES.
		 */
		inline bool AES()const
		{
			return m_internal.m_f_1_ECX[25];
		}
		/**
		 *\~english
		 *\return		The XSAVE instructions support.
		 *\~french
		 *\return		Le support des instructions XSAVE.
		 */
		inline bool XSAVE()const
		{
			return m_internal.m_f_1_ECX[26];
		}
		/**
		 *\~english
		 *\return		The OSXSAVE instructions support.
		 *\~french
		 *\return		Le support des instructions OSXSAVE.
		 */
		inline bool OSXSAVE()const
		{
			return m_internal.m_f_1_ECX[27];
		}
		/**
		 *\~english
		 *\return		The AVX instructions support.
		 *\~french
		 *\return		Le support des instructions AVX.
		 */
		inline bool AVX()const
		{
			return m_internal.m_f_1_ECX[28];
		}
		/**
		 *\~english
		 *\return		The F16C instructions support.
		 *\~french
		 *\return		Le support des instructions F16C.
		 */
		inline bool F16C()const
		{
			return m_internal.m_f_1_ECX[29];
		}
		/**
		 *\~english
		 *\return		The RDRAND instructions support.
		 *\~french
		 *\return		Le support des instructions RDRAND.
		 */
		inline bool RDRAND()const
		{
			return m_internal.m_f_1_ECX[30];
		}
		/**
		 *\~english
		 *\return		The MSR instructions support.
		 *\~french
		 *\return		Le support des instructions MSR.
		 */
		inline bool MSR()const
		{
			return m_internal.m_f_1_EDX[5];
		}
		/**
		 *\~english
		 *\return		The CX8 instructions support.
		 *\~french
		 *\return		Le support des instructions CX8.
		 */
		inline bool CX8()const
		{
			return m_internal.m_f_1_EDX[8];
		}
		/**
		 *\~english
		 *\return		The SEP instructions support.
		 *\~french
		 *\return		Le support des instructions SEP.
		 */
		inline bool SEP()const
		{
			return m_internal.m_f_1_EDX[11];
		}
		/**
		 *\~english
		 *\return		The CMOV instructions support.
		 *\~french
		 *\return		Le support des instructions CMOV.
		 */
		inline bool CMOV()const
		{
			return m_internal.m_f_1_EDX[15];
		}
		/**
		 *\~english
		 *\return		The CLFSH instructions support.
		 *\~french
		 *\return		Le support des instructions CLFSH.
		 */
		inline bool CLFSH()const
		{
			return m_internal.m_f_1_EDX[19];
		}
		/**
		 *\~english
		 *\return		The MMX instructions support.
		 *\~french
		 *\return		Le support des instructions MMX.
		 */
		inline bool MMX()const
		{
			return m_internal.m_f_1_EDX[23];
		}
		/**
		 *\~english
		 *\return		The FXSR instructions support.
		 *\~french
		 *\return		Le support des instructions FXSR.
		 */
		inline bool FXSR()const
		{
			return m_internal.m_f_1_EDX[24];
		}
		/**
		 *\~english
		 *\return		The SSE instructions support.
		 *\~french
		 *\return		Le support des instructions SSE.
		 */
		inline bool SSE()const
		{
			return m_internal.m_f_1_EDX[25];
		}
		/**
		 *\~english
		 *\return		The SSE2 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE2.
		 */
		inline bool SSE2()const
		{
			return m_internal.m_f_1_EDX[26];
		}
		/**
		 *\~english
		 *\return		The FSGSBASE instructions support.
		 *\~french
		 *\return		Le support des instructions FSGSBASE.
		 */
		inline bool FSGSBASE()const
		{
			return m_internal.m_f_7_EBX[0];
		}
		/**
		 *\~english
		 *\return		The BMI1 instructions support.
		 *\~french
		 *\return		Le support des instructions BMI1.
		 */
		inline bool BMI1()const
		{
			return m_internal.m_f_7_EBX[3];
		}
		/**
		 *\~english
		 *\return		The HLE instructions support.
		 *\~french
		 *\return		Le support des instructions HLE.
		 */
		inline bool HLE()const
		{
			return m_internal.m_isIntel && m_internal.m_f_7_EBX[4];
		}
		/**
		 *\~english
		 *\return		The AVX2 instructions support.
		 *\~french
		 *\return		Le support des instructions AVX2.
		 */
		inline bool AVX2()const
		{
			return m_internal.m_f_7_EBX[5];
		}
		/**
		 *\~english
		 *\return		The BMI2 instructions support.
		 *\~french
		 *\return		Le support des instructions BMI2.
		 */
		inline bool BMI2()const
		{
			return m_internal.m_f_7_EBX[8];
		}
		/**
		 *\~english
		 *\return		The ERMS instructions support.
		 *\~french
		 *\return		Le support des instructions ERMS.
		 */
		inline bool ERMS()const
		{
			return m_internal.m_f_7_EBX[9];
		}
		/**
		 *\~english
		 *\return		The INVPCID instructions support.
		 *\~french
		 *\return		Le support des instructions INVPCID.
		 */
		inline bool INVPCID()const
		{
			return m_internal.m_f_7_EBX[10];
		}
		/**
		 *\~english
		 *\return		The RTM instructions support.
		 *\~french
		 *\return		Le support des instructions RTM.
		 */
		inline bool RTM()const
		{
			return m_internal.m_isIntel && m_internal.m_f_7_EBX[11];
		}
		/**
		 *\~english
		 *\return		The AVX512F instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512F.
		 */
		inline bool AVX512F()const
		{
			return m_internal.m_f_7_EBX[16];
		}
		/**
		 *\~english
		 *\return		The RDSEED instructions support.
		 *\~french
		 *\return		Le support des instructions RDSEED.
		 */
		inline bool RDSEED()const
		{
			return m_internal.m_f_7_EBX[18];
		}
		/**
		 *\~english
		 *\return		The ADX instructions support.
		 *\~french
		 *\return		Le support des instructions ADX.
		 */
		inline bool ADX()const
		{
			return m_internal.m_f_7_EBX[19];
		}
		/**
		 *\~english
		 *\return		The AVX512PF instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512PF.
		 */
		inline bool AVX512PF()const
		{
			return m_internal.m_f_7_EBX[26];
		}
		/**
		 *\~english
		 *\return		The AVX512ER instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512ER.
		 */
		inline bool AVX512ER()const
		{
			return m_internal.m_f_7_EBX[27];
		}
		/**
		 *\~english
		 *\return		The AVX512CD instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512CD.
		 */
		inline bool AVX512CD()const
		{
			return m_internal.m_f_7_EBX[28];
		}
		/**
		 *\~english
		 *\return		The SHA instructions support.
		 *\~french
		 *\return		Le support des instructions SHA.
		 */
		inline bool SHA()const
		{
			return m_internal.m_f_7_EBX[29];
		}
		/**
		 *\~english
		 *\return		The PREFETCHWT1 instructions support.
		 *\~french
		 *\return		Le support des instructions PREFETCHWT1.
		 */
		inline bool PREFETCHWT1()const
		{
			return m_internal.m_f_7_ECX[0];
		}

	private:
		CU_API static CpuInformationsInternal const m_internal;
	};
	/**
	 *\~english
	 *\brief			Output stream operator.
	 *\param[in,out]	stream	The stream.
	 *\param[in]		object	The object to put in the stream.
	 *\return			The stream.
	 *\~french
	 *\brief			Opérateur de flux de sortie.
	 *\param[in,out]	stream	Le flux.
	 *\param[in]		object	L'objet à mettre dans le flux.
	 *\return			Le flux
	 */
	CU_API std::ostream & operator<<( std::ostream & stream, CpuInformations const & object );
}

#endif
