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
		: public ATL::CComObjectRootEx< ATL::CComSingleThreadModel >
		, public ATL::CComCoClass< CObject, ClsidClass >
		, public ATL::IDispatchImpl< IObject, IidObject, &LIBID_Castor3D, MAJOR_VERSION, MINOR_VERSION >
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
