/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageCache_H___
#define ___CU_ImageCache_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/Size.hpp"

#include "CastorUtils/Design/Resource.hpp"
#include "CastorUtils/Design/ResourceCache.hpp"

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
		enum Mode
		{
			eFile,
			eBuffer,
			eParam,
		};

		explicit ImageCreateParams( Path const & ppath
			, ImageLoaderConfig ploadConfig = { true, true, true } )
			: mode{ eFile }
			, path{ ppath }
			, loadConfig{ std::move( ploadConfig ) }
		{
		}

		explicit ImageCreateParams( String const & ptype
			, ByteArray const & pdata
			, ImageLoaderConfig ploadConfig = { true, true, true } )
			: mode{ eBuffer }
			, loadConfig{ std::move( ploadConfig ) }
			, type{ ptype }
			, data{ pdata }
		{
		}

		ImageCreateParams( Size const & psize
			, PixelFormat pformat )
			: mode{ eParam }
			, size{ psize }
			, format{ pformat }
		{
		}

		Mode mode;
		// mode 0
		Path path;
		ImageLoaderConfig loadConfig{};
		// mode 1
		String type;
		ByteArray data;
		// mode 2
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
	template<>
	struct ResourceCacheTraitsT< Image, String >
		: ResourceCacheTraitsBaseT< Image, String, ResourceCacheTraitsT< Image, String > >
	{
		using KeyT = String;
		using Base = ResourceCacheTraitsBaseT< Image, KeyT, ResourceCacheTraitsT< Image, KeyT > >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		CU_API static const String Name;

		CU_API static ElementPtrT makeElement( ResourceCacheBaseT< ElementT, KeyT, ResourceCacheTraitsT< ElementT, KeyT > > const & cache
			, KeyT const & name
			, ImageCreateParams const & params );
	};
	using ImageCacheTraits = ResourceCacheTraitsT< Image, String >;
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
	class ResourceCacheT< Image, String, ImageCacheTraits > final
		: public ResourceCacheBaseT< Image, String, ImageCacheTraits >
	{
	public:
		using ElementT = Image;
		using ElementKeyT = String;
		using ElementCacheTraitsT = ImageCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;

	public:
		CU_API explicit ResourceCacheT( LoggerInstance & logger
			, ImageLoader const & loader );
		CU_API ~ResourceCacheT() = default;

		ImageLoader const & getLoader()const
		{
			return m_loader;
		}

	private:
		ImageLoader const & m_loader;
	};

	using ImageCache = ResourceCacheT< Image, String, ImageCacheTraits >;
	using ImageRes = ImageCacheTraits::ElementPtrT;
	using ImageResPtr = ImageCacheTraits::ElementObsT;
}

#endif

