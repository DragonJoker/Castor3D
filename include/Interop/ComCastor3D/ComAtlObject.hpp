/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_ATL_OBJECT_H___
#define ___C3DCOM_COM_ATL_OBJECT_H___

#include "ComCastor3D/ComVariableGetter.hpp"
#include "ComCastor3D/ComVariablePutter.hpp"
#include "ComCastor3D/ComVariableIndexedGetter.hpp"
#include "ComCastor3D/ComVariableIndexedPutter.hpp"
#include "ComCastor3D/ComError.hpp"

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
