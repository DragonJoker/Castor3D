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

		static LPCOLESTR GetAppId() throw()
		{
			return T2CW( m_appId );
		}

		static TCHAR const * GetAppIdT() throw()
		{
			return m_appId;
		}

		static HRESULT WINAPI UpdateRegistryAppId( BOOL bRegister )throw()
		{
			ATL::_ATL_REGMAP_ENTRY aMapEntries [] =
			{
				{ OLESTR( "APPID" ), GetAppId() },
				{ nullptr, nullptr }
			};
			return ATL::_pAtlModule->UpdateRegistryFromResource( IDR_Castor3D, bRegister, aMapEntries );
		}

	private:
		static TCHAR const * m_appId;
	};

	extern class CComCastor3DModule _AtlModule;
}


#endif
