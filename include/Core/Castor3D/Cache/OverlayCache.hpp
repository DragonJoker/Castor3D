/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayCache_H___
#define ___C3D_OverlayCache_H___

#include "CacheModule.hpp"

#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayFactory.hpp"
#include "Castor3D/Render/Viewport.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace castor
{
	/**
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	template<>
	class ResourceCacheT< castor3d::Overlay, String, castor3d::OverlayCacheTraits > final
		: public ResourceCacheBaseT< castor3d::Overlay, String, castor3d::OverlayCacheTraits >
	{
	public:
		using ElementT = castor3d::Overlay;
		using ElementKeyT = String;
		using ElementCacheTraitsT = castor3d::OverlayCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		using iterator = std::map< ElementT, ElementKeyT >::iterator;
		using const_iterator = std::map< ElementT, ElementKeyT >::const_iterator;
		CU_DeclareMap( ElementKeyT, castor3d::FontTextureSPtr, FontTextureStr );
		using OverlayCategories = std::vector< castor3d::OverlayCategorySPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ResourceCacheT( castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ResourceCacheT()override = default;
		/**
		 *\~english
		 *\brief		Clears all overlays lists
		 *\~french
		 *\brief		Vide les listes d'incrustations
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Cleans all overlays up.
		 *\~french
		 *\brief		Nettoie les incrustations.
		 */
		C3D_API void cleanup();
		C3D_API void initialise( castor3d::Overlay & overlay );
		C3D_API void cleanup( castor3d::Overlay & overlay );
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a FontTexture given a font name.
		 *\param[in]	name	The font name.
		 *\return		The FontTexture if it exist, nullptr if not.
		 *\~french
		 *\brief		Récupère une FontTexture, à partir d'un nom de police.
		 *\param[in]	name	Le nom de la police.
		 *\return		La FontTexture si elle exite, nullptr sinon.
		 */
		C3D_API castor3d::FontTextureSPtr getFontTexture( String const & name );
		/**
		 *\~english
		 *\brief		Creates a FontTexture from a font.
		 *\param[in]	font	The font.
		 *\return		The created FontTexture.
		 *\~french
		 *\brief		Crée une FontTexture, à partir d'une police.
		 *\param[in]	font	La police.
		 *\return		La FontTexture créée.
		 */
		C3D_API castor3d::FontTextureSPtr createFontTexture( castor::FontResPtr font );
		/**
		*\~english
		*\name Iteration.
		*\~french
		*\name Itération.
		**/
		/**@{*/
		inline auto const & getCategories()const
		{
			return m_overlays;
		}
		/**@}*/
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		inline castor3d::OverlayFactory const & getOverlayFactory()const
		{
			return m_overlayFactory;
		}

		inline castor3d::OverlayFactory & getFactory()
		{
			return m_overlayFactory;
		}

		castor3d::Engine & getEngine()const
		{
			return m_engine;
		}
		/**@}*/

	private:
		castor3d::Engine & m_engine;
		castor3d::OverlayFactory m_overlayFactory;
		OverlayCategories m_overlays;
		castor3d::Viewport m_viewport;
		Matrix4x4f m_projection;
		FontTextureStrMap m_fontTextures;
		std::map< uint32_t, std::map< uint32_t, uint32_t > > m_overlayCountPerLevel;
	};
}

#endif
