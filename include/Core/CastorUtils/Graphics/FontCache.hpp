/*
See LICENSE file in root folder
*/
#ifndef ___CU_FontCache_H___
#define ___CU_FontCache_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Design/ResourceCacheBase.hpp"

#include <unordered_map>

namespace castor
{
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for castor::Font.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour castor::Font.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< Font, KeyT >
		: ResourceCacheTraitsBaseT< Font, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< Font, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		CU_API static const String Name;

		using ElementProducerT = std::function< ElementPtrT( KeyT const &
			, uint32_t
			, Path const & ) >;
	};
	/**
	*\~english
	*	Base class for an element cache.
	*\remarks
	*	Specialisation for castor::Font.
	*\~french
	*	Classe de base pour un cache d'éléments.
	*\remarks
	*	Spécialisation pour castor::Font.
	*/
	template<>
	class ResourceCacheT< Font, String > final
		: public ResourceCacheBaseT< Font, String >
	{
	public:
		using ElementT = Font;
		using ElementKeyT = String;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT >;
		using ElementCacheTraitsT = typename ElementCacheT::ElementCacheTraitsT;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementProducerT = typename ElementCacheT::ElementProducerT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;

	public:
		CU_API explicit ResourceCacheT( LoggerInstance & logger );
		CU_API ~ResourceCacheT() = default;

	private:
		FontSPtr doProduce( String const & name
			, uint32_t height
			, Path path );
		void doInitialise( FontSPtr resource );

	private:
		using PathNameMap = std::unordered_map< String, Path >;
		//!\~english	The font files paths sorted by file_name.file_extension.
		//!\~french		Les fichiers des polices, triés par file_name.file_extension.
		PathNameMap m_paths;
	};

	using FontCache = ResourceCacheT< Font, String >;
}

#endif
