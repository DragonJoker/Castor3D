/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
