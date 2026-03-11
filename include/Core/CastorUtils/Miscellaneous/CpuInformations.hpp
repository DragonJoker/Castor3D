/*
See LICENSE file in root folder
*/
#ifndef ___CU_CpuInformations_H___
#define ___CU_CpuInformations_H___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include <bitset>

namespace c3d
{
	namespace platform
	{
		uint32_t getCoreCount();
		String getCPUModel();
		String getCPUVendor();
	}
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
			String m_vendor{};
			String m_model{};
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
		String getVendor()const
		{
			return m_internal.m_vendor;
		}
		/**
		 *\~english
		 *\return		The CPU model.
		 *\~french
		 *\return		Le modèle du CPU.
		 */
		String getModel()const
		{
			return m_internal.m_model;
		}

	private:
		CU_API static CpuInformationsInternal const m_internal;

	private:
		friend OutputStream & operator<<( OutputStream & stream, CpuInformations const & object )
		{
			stream << "CPU informations:" << std::endl;
			stream << "    Vendor: " << object.getVendor() << std::endl;
			stream << "    Model: " << object.getModel() << std::endl;
			stream << "    Core count: " << object.getCoreCount();
			return stream;
		}
	};
}

#endif
