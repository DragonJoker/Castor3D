/* See LICENSE file in root folder */
#ifndef ___C3DCOM_CASTOR3D_COM_MODULE_H___
#define ___C3DCOM_CASTOR3D_COM_MODULE_H___

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

namespace CastorCom
{
	class CComCastor3DModule
		: public ATL::CAtlDllModuleT< CComCastor3DModule >
	{
	public :
		DECLARE_LIBID( LIBID_Castor3D )

		static LPCOLESTR etAppId()noexcept
		{
			return m_oleappId;
		}

		static TCHAR const * GetAppIdT()noexcept
		{
			return m_tappId;
		}

		static HRESULT WINAPI UpdateRegistryAppId( BOOL bRegister )noexcept
		{
			ATL::_ATL_REGMAP_ENTRY aMapEntries [] =
			{
				{ OLESTR( "APPID" ), GetAppId() },
				{ nullptr, nullptr }
			};
			return ATL::_pAtlModule->UpdateRegistryFromResource( IDR_Castor3D, bRegister, aMapEntries );
		}

	private:
		static TCHAR const * m_tappId;
		static OLECHAR const * m_oleappId;
	};

	extern class CComCastor3DModule _AtlModule;
}


#endif
