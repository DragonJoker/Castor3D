/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayCache_H___
#define ___C3D_OverlayCache_H___

#include "CacheModule.hpp"

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayFactory.hpp"
#include "Castor3D/Render/Viewport.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace c3d
{
	/**
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	template<>
	class ResourceCacheT< Overlay, String, OverlayCacheTraits > final
		: public ResourceCacheBaseT< Overlay, String, OverlayCacheTraits >
	{
	public:
		using ElementT = Overlay;
		using ElementKeyT = String;
		using ElementCacheTraitsT = OverlayCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		using iterator = Map< ElementT, ElementKeyT >::iterator;
		using const_iterator = Map< ElementT, ElementKeyT >::const_iterator;
		CU_DeclareMap( ElementKeyT, FontTextureUPtr, FontTextureStr );
		using OverlayCategories = Vector< OverlayCategoryRPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ResourceCacheT( Engine & engine );
		/**
		 *\~english
		 *\brief		Clears all overlays lists
		 *\~french
		 *\brief		Vide les listes d'incrustations
		 */
		C3D_API void clear()noexcept;
		/**
		 *\~english
		 *\brief		Cleans all overlays up.
		 *\~french
		 *\brief		Nettoie les incrustations.
		 */
		C3D_API void cleanup();
		C3D_API void initialise( Overlay & overlay );
		C3D_API void cleanup( Overlay & overlay );
		C3D_API void upload( UploadData & uploader );
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
		C3D_API FontTextureRPtr getFontTexture( String const & name );
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
		C3D_API FontTextureRPtr createFontTexture( FontResPtr font );
		/**
		*\~english
		*\name Iteration.
		*\~french
		*\name Itération.
		**/
		/**@{*/
		auto const & getCategories()const noexcept
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
		OverlayFactory const & getOverlayFactory()const noexcept
		{
			return m_overlayFactory;
		}

		OverlayFactory & getFactory()noexcept
		{
			return m_overlayFactory;
		}

		Engine & getEngine()const noexcept
		{
			return m_engine;
		}
		/**@}*/

	private:
		Engine & m_engine;
		OverlayFactory m_overlayFactory;
		OverlayCategories m_overlays;
		Viewport m_viewport;
		Matrix4x4f m_projection;
		FontTextureStrMap m_fontTextures;
		Map< uint32_t, Map< uint32_t, uint32_t > > m_overlayCountPerLevel;
	};
}

#endif
