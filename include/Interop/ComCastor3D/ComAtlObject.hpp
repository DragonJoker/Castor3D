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
	namespace details
	{
	}

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
	class CComAtlObject
		: public CComAtlObjectBaseT< Class, Object >
	{
	protected:
		using Internal = ComTypeInternalT< Object >;
		using InternalMbr = ComTypeInternalMbrT< Object >;
		using GetInternal = ComTypeGetInternalT< Object >;
		using SetInternal = ComTypeSetInternalT< Object >;

		InternalMbr m_internal{};

	public:
		inline GetInternal getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( SetInternal internal )
		{
			m_internal = std::move( internal );
		}
	};

#define CONCAT( x, y ) x ## y
#define STRINGIFY( x ) TEXT( #x )

#define COM_TYPE_TRAITS_EX( Name, Nmspc, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Nmspc::Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Nmspc::Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Nmspc::Class;\
		using InternalMbr = Internal;\
		using GetInternal = Internal;\
		using SetInternal = Internal;\
	}

#define COM_TYPE_TRAITS_PTR_EX( Name, Nmspc, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Nmspc::Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Nmspc::Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Nmspc::Class;\
		using InternalMbr = Internal *;\
		using GetInternal = Internal *;\
		using SetInternal = Internal *;\
	}

#define COM_TYPE_TRAITS_UPTR_EX( Name, Nmspc, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Nmspc::Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Nmspc::Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Nmspc::Class;\
		using InternalMbr = castor::UniquePtr< Internal >;\
		using GetInternal = Internal *;\
		using SetInternal = Internal *;\
	}

#define COM_TYPE_TRAITS_UUPTR_EX( Name, Nmspc, Class )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Nmspc::Class;\
	};\
	template<>\
	struct ComTypeTraitsT< Nmspc::Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Nmspc::Class;\
		using InternalMbr = castor::UniquePtr< Internal >;\
		using GetInternal = Internal *;\
		using SetInternal = castor::UniquePtr< Internal >;\
	}

#define COM_TYPE_TRAITS_RES_EX( Name, Nmspc, Class, Key )\
	class C##Name;\
	template<>\
	struct ComITypeTraitsT< I##Name >\
	{\
		static constexpr bool hasIType = true;\
		using Type = Nmspc::Class;\
		using GetSrcType = I##Name;\
		using GetDstType = C##Name;\
	};\
	template<>\
	struct ComTypeTraitsT< Nmspc::Class >\
	{\
		static constexpr bool hasIType = true;\
		static constexpr bool hasType = true;\
		static constexpr bool hasInternalType = true;\
		using IType = I##Name;\
		using CType = C##Name;\
		static inline const CLSID clsid = CLSID_##Name;\
		static inline const CLSID iid = IID_I##Name;\
		static inline const UINT rid = IDR_##Name;\
		using Internal = Nmspc::Class;\
		using InternalMbr = castor::ResourceObsT< Internal, Key >;\
		using GetInternal = castor::ResourceObsT< Internal, Key >;\
		using SetInternal = castor::ResourceObsT< Internal, Key >;\
	}

#define COM_TYPE_TRAITS( Nmspc, Class ) COM_TYPE_TRAITS_EX( Class, Nmspc, Class )
#define COM_TYPE_TRAITS_PTR( Nmspc, Class ) COM_TYPE_TRAITS_PTR_EX( Class, Nmspc, Class )
#define COM_TYPE_TRAITS_UPTR( Nmspc, Class ) COM_TYPE_TRAITS_UPTR_EX( Class, Nmspc, Class )
#define COM_TYPE_TRAITS_UUPTR( Nmspc, Class ) COM_TYPE_TRAITS_UUPTR_EX( Class, Nmspc, Class )
#define COM_TYPE_TRAITS_RES( Nmspc, Class, Key ) COM_TYPE_TRAITS_RES_EX( Class, Nmspc, Class, Key )

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

#define COM_PROPERTY( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, Getter )\
	COM_PROPERTY_PUT( Name, Type, Putter )

#define COM_EVT_PROPERTY( Name, Type, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, Getter )\
	COM_PROPERTY_PUT( Name, Type, Putter )

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

#define COMEX_PROPERTY_GET( Name, Type, Instance, Functor )\
	COM_PROPERTY_GET( Name, Type, makeGetter( Instance, Functor ) )

#define COMEX_PROPERTY_PUT( Name, Type, Instance, Functor )\
	COM_PROPERTY_PUT( Name, Type, makePutter( Instance, Functor ) )

#define COMEX_PROPERTY( Name, Type, Instance, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, makeGetter( Instance, Getter ) )\
	COM_PROPERTY_PUT( Name, Type, makePutter( Instance, Putter ) )

#define COMEX_PROPERTY_IDX( Name, Index, Type, Instance, Getter, Putter )\
	COM_PROPERTY_GET( Name, Type, makeGetter( Instance, Getter, Index ) )\
	COM_PROPERTY_PUT( Name, Type, makePutter( Instance, Putter, Index ) )
}

#endif
