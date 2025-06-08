/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_ATL_OBJECT_H___
#define ___C3DCOM_COM_ATL_OBJECT_H___

#include "ComCastor3D/ComVariableGetter.hpp"
#include "ComCastor3D/ComVariablePutter.hpp"
#include "ComCastor3D/ComError.hpp"

namespace CastorCom
{
	template< typename Class, typename Object >
	class CComAtlObjectBaseT
		: public ATL::CComObjectRootEx< ATL::CComSingleThreadModel >
		, public ATL::CComCoClass< ComTypeCTypeT< Object >, &ComTypeClsidT< Object > >
		, public ATL::IDispatchImpl< ComTypeITypeT< Object >, &ComTypeIidT< Object >, &LIBID_Castor3D, MAJOR_VERSION, MINOR_VERSION >
	{
	protected:
		using CObject = ComTypeCTypeT< Object >;
		using IObject = ComTypeITypeT< Object >;

	public:
		CComAtlObjectBaseT() = default;
		CComAtlObjectBaseT( CComAtlObjectBaseT const & ) = default;
		CComAtlObjectBaseT( CComAtlObjectBaseT && ) = default;
		CComAtlObjectBaseT & operator=( CComAtlObjectBaseT const & ) = default;
		CComAtlObjectBaseT & operator=( CComAtlObjectBaseT && ) = default;
		virtual ~CComAtlObjectBaseT() = default;

		inline HRESULT FinalConstruct()
		{
			return S_OK;
		}

		inline void FinalRelease()
		{
		}

		DECLARE_REGISTRY_RESOURCEID( ComTypeRidT< Object > )
		BEGIN_COM_MAP( CObject )
			COM_INTERFACE_ENTRY( IObject )
			COM_INTERFACE_ENTRY( IDispatch )
		END_COM_MAP()
		DECLARE_PROTECT_FINAL_CONSTRUCT()
	};

	template< typename Class, typename Object >
	class CComAtlObjectT
		: public CComAtlObjectBaseT< Class, Object >
	{
	protected:
		using Internal = ComTypeInternalT< Object >;
		using InternalPtr = ComTypeInternalPtrT< Object >;
		using InternalMbr = ComTypeInternalMbrT< Object >;
		using GetConstInternal = ComTypeGetConstInternalT < Object >;
		using GetInternal = ComTypeGetInternalT< Object >;
		using SetInternal = ComTypeSetInternalT< Object >;

		InternalMbr m_internal{};

	public:
		GetConstInternal getInternal()const
		{
			return m_internal;
		}

		GetInternal getInternal()
		{
			return m_internal;
		}

		void setInternal( SetInternal internal )
		{
			m_internal = std::move( internal );
		}

	protected:
		HRESULT dispatchError( HRESULT hr
			, LPCTSTR name
			, LPCTSTR message )
		{
			return CComError::dispatchError( hr, ComTypeIidT< Internal >, name, message, 0, nullptr );
		}

		HRESULT dispatchInitialised( LPCTSTR name )
		{
			return dispatchError( E_POINTER, name, _T( "The object was already initialised" ) );
		}

		HRESULT dispatchUninitialised( LPCTSTR name )
		{
			return dispatchError( E_POINTER, name, _T( "The object must be initialised" ) );
		}
	};

#define CONCAT( x, y ) x ## y
#define STRINGIFY( x ) TEXT( #x )

#define COM_TYPE_TRAITS( Name, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Class;\
		using InternalPtr = Class *;\
		using InternalMbr = Internal;\
		using GetConstInternal = Internal const &;\
		using GetInternal = Internal &;\
		using SetInternal = Internal const &;\
	}

#define COM_TYPE_TRAITS_PTR( Name, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Class;\
		using InternalPtr = Class *;\
		using InternalMbr = InternalPtr;\
		using GetConstInternal= Internal const *;\
		using GetInternal = Internal *&;\
		using SetInternal = Internal *;\
	}

#define COM_PROPERTY_GET_EX( Name, SrcType, DstType, Functor )\
	STDMETHODIMP CONCAT( get_, Name )( /* [out, retval] */ SrcType * pRet )override\
	{\
		return getValue< InternalPtr, DstType >( STRINGIFY( Name ), Functor, pRet, m_internal );\
	}

#define COM_PROPERTY_PUT_EX( Name, SrcType, DstType, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /*[in]*/ SrcType val )override\
	{\
		return putValue< InternalPtr, DstType >( STRINGIFY( Name ), Functor, val, m_internal );\
	}

#define COM_PROPERTY_EX( Name, SrcType, DstType, Getter, Putter )\
	COM_PROPERTY_GET_EX( Name, SrcType, DstType, Getter )\
	COM_PROPERTY_PUT_EX( Name, SrcType, DstType, Putter )

#define COM_PROPERTY_GET( Name, Type, Functor )\
	COM_PROPERTY_GET_EX( Name, Type, Type, Functor )

#define COM_PROPERTY_PUT( Name, Type, Functor )\
	COM_PROPERTY_PUT_EX( Name, Type, Type, Functor )

#define COM_PROPERTY( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, Getter )\
	COM_PROPERTY_PUT( Name, Type, Putter )

#define COM_PROPERTY_GET_SPTR( Name, Type, Functor )\
	STDMETHODIMP CONCAT( get_, Name )( /* [out, retval] */ Type ** pRet )override\
	{\
		return getValueSimplePtr< InternalPtr >( STRINGIFY( Name ), Functor, pRet, m_internal );\
	}

#define COM_PROPERTY_PUT_SPTR( Name, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /*[in]*/ Type * val )override\
	{\
		return putValueSimplePtr< InternalPtr >( STRINGIFY( Name ), Functor, val, m_internal );\
	}

#define COM_PROPERTY_SPTR( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET_SPTR( Name, Type, Getter )\
	COM_PROPERTY_PUT_SPTR( Name, Type, Putter )

#define COM_PROPERTY_GET_MPTR( Name, Type, Functor )\
	STDMETHODIMP CONCAT( get_, Name )( /* [out, retval] */ Type ** pRet )override\
	{\
		return getValueManagedPtr< InternalPtr >( STRINGIFY( Name ), Functor, pRet, m_internal );\
	}

#define COM_PROPERTY_PUT_MPTR( Name, Type, Functor )\
	STDMETHODIMP CONCAT( put_, Name )( /*[in]*/ Type * val )override\
	{\
		return putValueManagedPtr< InternalPtr >( STRINGIFY( Name ), Functor, val, m_internal );\
	}

#define COM_PROPERTY_MPTR( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET_MPTR( Name, Type, Getter )\
	COM_PROPERTY_PUT_MPTR( Name, Type, Putter )

#define COM_PROPERTY_GET_MBR( Name, Type, PtrValue )\
	STDMETHODIMP CONCAT( get_, Name )( /* [out, retval] */ Type * pRet )override\
	{\
		if ( !pRet )\
			return E_POINTER;\
		*pRet = *PtrValue;\
		return S_OK;\
	}

#define COM_PROPERTY_PUT_MBR( Name, Type, PtrValue )\
	STDMETHODIMP CONCAT( put_, Name )( /*[in]*/ Type val )override\
	{\
		*PtrValue = val;\
		return S_OK;\
	}

#define COM_PROPERTY_MBR( Name, Type, PtrValue )\
	COM_PROPERTY_GET_MBR( Name, Type, PtrValue )\
	COM_PROPERTY_PUT_MBR( Name, Type, PtrValue )

#define COM_DESTROY( Class, Functor )\
	~Class()noexcept\
	{\
		destroyInternal( Functor, m_internal );\
	}
}

#endif
