/*
See LICENSE file in root folder
*/
#ifndef ___CU_FontCache_H___
#define ___CU_FontCache_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Graphics/Font.hpp"

#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Design/Resource.hpp"
#include "CastorUtils/Design/ResourceCache.hpp"

#include <unordered_map>

namespace castor
{
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
	class ResourceCacheT< Font, String, FontCacheTraits > final
		: public ResourceCacheBaseT< Font, String, FontCacheTraits >
	{
	public:
		using ElementT = Font;
		using ElementKeyT = String;
		using ElementCacheTraitsT = FontCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;

	public:
		CU_API explicit ResourceCacheT( LoggerInstance & logger );
		CU_API ~ResourceCacheT()noexcept override = default;

		CU_API Path getRealPath( Path path )const;

	private:
		using PathNameMap = UnorderedStringMap< Path >;
		//!\~english	The font files paths sorted by file_name.file_extension.
		//!\~french		Les fichiers des polices, triés par file_name.file_extension.
		PathNameMap m_paths;
	};

	using FontRes = FontCacheTraits::ElementPtrT;
	using FontResPtr = FontCacheTraits::ElementObsT;
}

#endif
