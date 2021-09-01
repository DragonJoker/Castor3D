/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageCache_H___
#define ___CU_ImageCache_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Design/ResourceCacheBase.hpp"

namespace castor
{
	/**
	*\~english
	*	castor::Image creation parameters.
	*\~french
	*	Paramètres de création d'une castor::Image.
	*/
	struct ImageCreateParams
	{
		ImageCreateParams( Path const & path
			, bool allowCompression = true
			, bool generateMips = true )
			: mode{ false }
			, path{ path }
			, allowCompression{ allowCompression }
			, generateMips{ generateMips }
		{
		}

		ImageCreateParams( Size const & size
			, PixelFormat format )
			: mode{ true }
			, size{ size }
			, format{ format }
		{
		}

		bool mode;
		// mode 0
		Path path;
		bool allowCompression{};
		bool generateMips{};
		// mode 1
		Size size{};
		PixelFormat format{};
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for castor::Image.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour castor::Image.
	*/
	template< typename KeyT >
	struct ResourceCacheTraitsT< Image, KeyT >
		: ResourceCacheTraitsBaseT< Image, KeyT >
	{
		using Base = ResourceCacheTraitsBaseT< Image, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		CU_API static const String Name;

		using ElementProducerT = std::function< typename ElementPtrT( KeyT const &
			, ImageCreateParams const & ) >;
	};
	/**
	*\~english
	*	Base class for an element cache.
	*\remarks
	*	Specialisation for castor::Image.
	*\~french
	*	Classe de base pour un cache d'éléments.
	*\remarks
	*	Spécialisation pour castor::Image.
	*/
	template<>
	class ResourceCacheT< Image, String > final
		: public ResourceCacheBaseT< Image, String >
	{
	public:
		using ElementT = Image;
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
		CU_API explicit ResourceCacheT( LoggerInstance & logger
			, ImageLoader const & loader );
		CU_API ~ResourceCacheT() = default;

	private:
		ImageSPtr doProduce( String const & name
			, ImageCreateParams const & params );
		void doInitialise( ImageSPtr resource );

	private:
		ImageLoader const & m_loader;
	};

	using ImageCache = ResourceCacheT< Image, String >;
}

#endif

