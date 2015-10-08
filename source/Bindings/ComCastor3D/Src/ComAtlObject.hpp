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
#ifndef ___C3DCOM_COM_ATL_OBJECT_H___
#define ___C3DCOM_COM_ATL_OBJECT_H___

#include "ComVariableGetter.hpp"
#include "ComVariablePutter.hpp"
#include "ComVariableIndexedGetter.hpp"
#include "ComVariableIndexedPutter.hpp"
#include "ComError.hpp"

namespace CastorCom
{
	template< typename Class, const CLSID * ClsidClass, typename CObject, typename IObject, const CLSID * IidObject, UINT Resource >
	class CComAtlObject
		:	public ATL::CComObjectRootEx< ATL::CComSingleThreadModel >
		,	public ATL::CComCoClass< CObject, ClsidClass >
		,	public ATL::IDispatchImpl< IObject, IidObject, &LIBID_Castor3D, MAJOR_VERSION, MINOR_VERSION >
	{
	public:
		CComAtlObject()
		{
		}

		~CComAtlObject()
		{
		}

		DECLARE_REGISTRY_RESOURCEID( Resource )
		BEGIN_COM_MAP( CObject )
		COM_INTERFACE_ENTRY( IObject )
		COM_INTERFACE_ENTRY( IDispatch )
		END_COM_MAP()
		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}
		void FinalRelease()
		{
		}
	};

#define CONCAT( x, y ) x ## y
#define STRINGIFY( x ) TEXT( #x )

#define COM_ATL_OBJECT( Name ) public CComAtlObject< Name, &CONCAT( CLSID_, Name ), CONCAT( C, Name ), CONCAT( I, Name ), &CONCAT( IID_I, Name ), CONCAT( IDR_, Name ) >

#define COM_PROPERTY_GET( Name, Type, Functor )\
	STDMETHODIMP CONCAT( get_, Name )( /* [retval][out] */ Type * pVal )\
	{\
		return Functor( pVal );\
	}

#define COM_PROPERTY_PUT( Name, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /* [in] */ Type val )\
	{\
		return Functor( val );\
	}

#define COM_EVT_PROPERTY_PUT( Name, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /* [in] */ Type val )\
	{\
		return Functor( val );\
	}

#define COM_PROPERTY( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, Getter )\
	COM_PROPERTY_PUT( Name, Type, Putter )

#define COM_EVT_PROPERTY( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, Getter )\
	COM_EVT_PROPERTY_PUT( Name, Type, Putter )

#define COM_PROPERTY_INDEXED_GET( Name, Index, Type, Functor )\
	STDMETHODIMP CONCAT( get_, Name )( /* [in] */ Index index, /* [retval][out] */ Type * pVal )\
	{\
		return Functor( index, pVal );\
	}

#define COM_PROPERTY_INDEXED_PUT( Name, Index, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /* [in] */ Index index, /* [in] */ Type val )\
	{\
		return Functor( index, val );\
	}

#define COM_EVT_PROPERTY_INDEXED_PUT( Name, Index, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /* [in] */ Index index, /* [in] */ Type val )\
	{\
		return Functor( index, val );\
	}

#define COM_PROPERTY_INDEXED( Name, Index, Type, Getter, Putter )\
	COM_PROPERTY_INDEXED_GET( Name, Index, Type, Getter )\
	COM_PROPERTY_INDEXED_PUT( Name, Index, Type, Putter )

#define COM_EVT_PROPERTY_INDEXED( Name, Index, Type, Getter, Putter )\
	COM_PROPERTY_INDEXED_GET( Name, Index, Type, Getter )\
	COM_EVT_PROPERTY_INDEXED_PUT( Name, Index, Type, Putter )
}

#endif
