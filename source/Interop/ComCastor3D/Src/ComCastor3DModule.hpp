/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DCOM_CASTOR3D_COM_MODULE_H___
#define ___C3DCOM_CASTOR3D_COM_MODULE_H___

#include "ComCastor3DPrerequisites.hpp"

namespace CastorCom
{
	class CComCastor3DModule
		: public ATL::CAtlDllModuleT< CComCastor3DModule >
	{
	public :
		DECLARE_LIBID( LIBID_Castor3D )

		static LPCOLESTR GetAppId() throw()
		{
			USES_CONVERSION;
			return T2W( m_appId );
		}

		static TCHAR * GetAppIdT() throw()
		{
			return m_appId;
		}

		static HRESULT WINAPI UpdateRegistryAppId( BOOL bRegister )throw()
		{
			ATL::_ATL_REGMAP_ENTRY aMapEntries [] =
			{
				{ OLESTR( "APPID" ), GetAppId() },
				{ NULL, NULL }
			};
			return ATL::_pAtlModule->UpdateRegistryFromResource( IDR_Castor3D, bRegister, aMapEntries );
		}

	private:
		static TCHAR * m_appId;
	};

	extern class CComCastor3DModule _AtlModule;
}


#endif
