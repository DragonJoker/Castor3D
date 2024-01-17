/*
See LICENSE file in root folder
*/
#ifndef ___CU_CpuInformations_H___
#define ___CU_CpuInformations_H___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include <bitset>

namespace castor
{
	/**
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
			std::string m_model{};
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
		CU_API CpuInformations() = default;
		/**
		 *\~english
		 *\return		The number of cores (Physical + Virtual).
		 *\~french
		 *\return		Le nombre de coeurs (Physiques + Virtuels).
		 */
		uint32_t getCoreCount()const
		{
			return m_internal.m_coreCount;
		}
		/**
		 *\~english
		 *\return		The CPU vendor.
		 *\~french
		 *\return		Le vendeur du CPU.
		 */
		std::string getVendor()const
		{
			return m_internal.m_vendor;
		}
		/**
		 *\~english
		 *\return		The CPU model.
		 *\~french
		 *\return		Le modèle du CPU.
		 */
		std::string getModel()const
		{
			return m_internal.m_model;
		}
		/**
		 *\~english
		 *\return		The SSE3 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE3.
		 */
		bool SSE3()const
		{
			return m_internal.m_f_1_ECX[0];
		}
		/**
		 *\~english
		 *\return		The PCLMULQDQ instructions support.
		 *\~french
		 *\return		Le support des instructions PCLMULQDQ.
		 */
		bool PCLMULQDQ()const
		{
			return m_internal.m_f_1_ECX[1];
		}
		/**
		 *\~english
		 *\return		The MONITOR instructions support.
		 *\~french
		 *\return		Le support des instructions MONITOR.
		 */
		bool MONITOR()const
		{
			return m_internal.m_f_1_ECX[3];
		}
		/**
		 *\~english
		 *\return		The SSSE3 instructions support.
		 *\~french
		 *\return		Le support des instructions SSSE3.
		 */
		bool SSSE3()const
		{
			return m_internal.m_f_1_ECX[9];
		}
		/**
		 *\~english
		 *\return		The FMA instructions support.
		 *\~french
		 *\return		Le support des instructions FMA.
		 */
		bool FMA()const
		{
			return m_internal.m_f_1_ECX[12];
		}
		/**
		 *\~english
		 *\return		The CMPXCHG16B instructions support.
		 *\~french
		 *\return		Le support des instructions CMPXCHG16B.
		 */
		bool CMPXCHG16B()const
		{
			return m_internal.m_f_1_ECX[13];
		}
		/**
		 *\~english
		 *\return		The SSE4.1 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE4.1.
		 */
		bool SSE41()const
		{
			return m_internal.m_f_1_ECX[19];
		}
		/**
		 *\~english
		 *\return		The SSE4.2 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE4.2.
		 */
		bool SSE42()const
		{
			return m_internal.m_f_1_ECX[20];
		}
		/**
		 *\~english
		 *\return		The MOVBE instructions support.
		 *\~french
		 *\return		Le support des instructions MOVBE.
		 */
		bool MOVBE()const
		{
			return m_internal.m_f_1_ECX[22];
		}
		/**
		 *\~english
		 *\return		The POPCNT instructions support.
		 *\~french
		 *\return		Le support des instructions POPCNT.
		 */
		bool POPCNT()const
		{
			return m_internal.m_f_1_ECX[23];
		}
		/**
		 *\~english
		 *\return		The AES instructions support.
		 *\~french
		 *\return		Le support des instructions AES.
		 */
		bool AES()const
		{
			return m_internal.m_f_1_ECX[25];
		}
		/**
		 *\~english
		 *\return		The XSAVE instructions support.
		 *\~french
		 *\return		Le support des instructions XSAVE.
		 */
		bool XSAVE()const
		{
			return m_internal.m_f_1_ECX[26];
		}
		/**
		 *\~english
		 *\return		The OSXSAVE instructions support.
		 *\~french
		 *\return		Le support des instructions OSXSAVE.
		 */
		bool OSXSAVE()const
		{
			return m_internal.m_f_1_ECX[27];
		}
		/**
		 *\~english
		 *\return		The AVX instructions support.
		 *\~french
		 *\return		Le support des instructions AVX.
		 */
		bool AVX()const
		{
			return m_internal.m_f_1_ECX[28];
		}
		/**
		 *\~english
		 *\return		The F16C instructions support.
		 *\~french
		 *\return		Le support des instructions F16C.
		 */
		bool F16C()const
		{
			return m_internal.m_f_1_ECX[29];
		}
		/**
		 *\~english
		 *\return		The RDRAND instructions support.
		 *\~french
		 *\return		Le support des instructions RDRAND.
		 */
		bool RDRAND()const
		{
			return m_internal.m_f_1_ECX[30];
		}
		/**
		 *\~english
		 *\return		The MSR instructions support.
		 *\~french
		 *\return		Le support des instructions MSR.
		 */
		bool MSR()const
		{
			return m_internal.m_f_1_EDX[5];
		}
		/**
		 *\~english
		 *\return		The CX8 instructions support.
		 *\~french
		 *\return		Le support des instructions CX8.
		 */
		bool CX8()const
		{
			return m_internal.m_f_1_EDX[8];
		}
		/**
		 *\~english
		 *\return		The SEP instructions support.
		 *\~french
		 *\return		Le support des instructions SEP.
		 */
		bool SEP()const
		{
			return m_internal.m_f_1_EDX[11];
		}
		/**
		 *\~english
		 *\return		The CMOV instructions support.
		 *\~french
		 *\return		Le support des instructions CMOV.
		 */
		bool CMOV()const
		{
			return m_internal.m_f_1_EDX[15];
		}
		/**
		 *\~english
		 *\return		The CLFSH instructions support.
		 *\~french
		 *\return		Le support des instructions CLFSH.
		 */
		bool CLFSH()const
		{
			return m_internal.m_f_1_EDX[19];
		}
		/**
		 *\~english
		 *\return		The MMX instructions support.
		 *\~french
		 *\return		Le support des instructions MMX.
		 */
		bool MMX()const
		{
			return m_internal.m_f_1_EDX[23];
		}
		/**
		 *\~english
		 *\return		The FXSR instructions support.
		 *\~french
		 *\return		Le support des instructions FXSR.
		 */
		bool FXSR()const
		{
			return m_internal.m_f_1_EDX[24];
		}
		/**
		 *\~english
		 *\return		The SSE instructions support.
		 *\~french
		 *\return		Le support des instructions SSE.
		 */
		bool SSE()const
		{
			return m_internal.m_f_1_EDX[25];
		}
		/**
		 *\~english
		 *\return		The SSE2 instructions support.
		 *\~french
		 *\return		Le support des instructions SSE2.
		 */
		bool SSE2()const
		{
			return m_internal.m_f_1_EDX[26];
		}
		/**
		 *\~english
		 *\return		The FSGSBASE instructions support.
		 *\~french
		 *\return		Le support des instructions FSGSBASE.
		 */
		bool FSGSBASE()const
		{
			return m_internal.m_f_7_EBX[0];
		}
		/**
		 *\~english
		 *\return		The BMI1 instructions support.
		 *\~french
		 *\return		Le support des instructions BMI1.
		 */
		bool BMI1()const
		{
			return m_internal.m_f_7_EBX[3];
		}
		/**
		 *\~english
		 *\return		The HLE instructions support.
		 *\~french
		 *\return		Le support des instructions HLE.
		 */
		bool HLE()const
		{
			return m_internal.m_isIntel && m_internal.m_f_7_EBX[4];
		}
		/**
		 *\~english
		 *\return		The AVX2 instructions support.
		 *\~french
		 *\return		Le support des instructions AVX2.
		 */
		bool AVX2()const
		{
			return m_internal.m_f_7_EBX[5];
		}
		/**
		 *\~english
		 *\return		The BMI2 instructions support.
		 *\~french
		 *\return		Le support des instructions BMI2.
		 */
		bool BMI2()const
		{
			return m_internal.m_f_7_EBX[8];
		}
		/**
		 *\~english
		 *\return		The ERMS instructions support.
		 *\~french
		 *\return		Le support des instructions ERMS.
		 */
		bool ERMS()const
		{
			return m_internal.m_f_7_EBX[9];
		}
		/**
		 *\~english
		 *\return		The INVPCID instructions support.
		 *\~french
		 *\return		Le support des instructions INVPCID.
		 */
		bool INVPCID()const
		{
			return m_internal.m_f_7_EBX[10];
		}
		/**
		 *\~english
		 *\return		The RTM instructions support.
		 *\~french
		 *\return		Le support des instructions RTM.
		 */
		bool RTM()const
		{
			return m_internal.m_isIntel && m_internal.m_f_7_EBX[11];
		}
		/**
		 *\~english
		 *\return		The AVX512F instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512F.
		 */
		bool AVX512F()const
		{
			return m_internal.m_f_7_EBX[16];
		}
		/**
		 *\~english
		 *\return		The RDSEED instructions support.
		 *\~french
		 *\return		Le support des instructions RDSEED.
		 */
		bool RDSEED()const
		{
			return m_internal.m_f_7_EBX[18];
		}
		/**
		 *\~english
		 *\return		The ADX instructions support.
		 *\~french
		 *\return		Le support des instructions ADX.
		 */
		bool ADX()const
		{
			return m_internal.m_f_7_EBX[19];
		}
		/**
		 *\~english
		 *\return		The AVX512PF instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512PF.
		 */
		bool AVX512PF()const
		{
			return m_internal.m_f_7_EBX[26];
		}
		/**
		 *\~english
		 *\return		The AVX512ER instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512ER.
		 */
		bool AVX512ER()const
		{
			return m_internal.m_f_7_EBX[27];
		}
		/**
		 *\~english
		 *\return		The AVX512CD instructions support.
		 *\~french
		 *\return		Le support des instructions AVX512CD.
		 */
		bool AVX512CD()const
		{
			return m_internal.m_f_7_EBX[28];
		}
		/**
		 *\~english
		 *\return		The SHA instructions support.
		 *\~french
		 *\return		Le support des instructions SHA.
		 */
		bool SHA()const
		{
			return m_internal.m_f_7_EBX[29];
		}
		/**
		 *\~english
		 *\return		The PREFETCHWT1 instructions support.
		 *\~french
		 *\return		Le support des instructions PREFETCHWT1.
		 */
		bool PREFETCHWT1()const
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
