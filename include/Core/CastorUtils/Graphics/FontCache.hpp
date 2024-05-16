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
		/**
		 *\~english
		 *\brief		Creates a non SDF font.
		 *\param[in]	name	The font name.
		 *\param[in]	height	The font char height.
		 *\param[in]	path	The font file path.
		 *\~french
		 *\brief		Crée une police non SDF.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	height	La hauteur des caractères de la police.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 */
		CU_API ElementPtrT create( String const & name
			, uint32_t height
			, Path const & path )const;
		/**
		 *\~english
		 *\brief		Creates an SDF font.
		 *\param[in]	name	The font name.
		 *\param[in]	path	The font file path.
		 *\~french
		 *\brief		Crée une police SDF.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 */
		CU_API ElementPtrT create( String const & name
			, Path const & path )const;
		/**
		 *\see		ResourceCacheBaseT< Font, String, FontCacheTraits >::add
		 */
		CU_API ElementObsT add( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true );
		/**
		 *\~english
		 *\brief		Creates and adds a non SDF font to the cache.
		 *\param[in]	name	The font name.
		 *\param[in]	height	The font char height.
		 *\param[in]	path	The font file path.
		 *\return		The real element (added or duplicate original ).
		 *\~french
		 *\brief		Crée et ajoute au cache une police non SDF.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	height	La hauteur des caractères de la police.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 *\return		L'élément réel (ajouté, ou original du doublon).
		 */
		CU_API ElementObsT add( String const & name
			, uint32_t height
			, Path const & path );
		/**
		 *\~english
		 *\brief		Creates and adds an SDF font to the cache.
		 *\param[in]	name	The font name.
		 *\param[in]	path	The font file path.
		 *\return		The real element (added or duplicate original ).
		 *\~french
		 *\brief		Crée et ajoute au cache une police SDF.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 *\return		L'élément réel (ajouté, ou original du doublon).
		 */
		CU_API ElementObsT add( String const & name
			, Path const & path );

	private:
		template< typename ... ParametersT >
		ElementObsT doTryAddNoLockT( ElementKeyT const & name
			, bool initialise
			, ElementObsT & created
			, ParametersT && ... parameters )
		{
			auto ires = m_resources.emplace( name, ElementPtrT{} );

			if ( ires.second )
			{
				ires.first->second = this->create( name
					, castor::forward< ParametersT >( parameters )... );
				created = ElementCacheTraitsT::makeElementObs( ires.first->second );

				if ( initialise
					&& m_initialise
					&& !ElementCacheTraitsT::isElementObsNull( created ) )
				{
					m_initialise( *ires.first->second );
				}
			}

			return ElementCacheTraitsT::makeElementObs( ires.first->second );
		}

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
