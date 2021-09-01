/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayCache_H___
#define ___C3D_OverlayCache_H___

#include "CacheModule.hpp"

#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayFactory.hpp"
#include "Castor3D/Render/Viewport.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

namespace castor
{
	/**
	\~english
	\brief		Structure used to sort overlays by order.
	\~french
	\brief		Structure utilisée pour trier les incrustations par ordre.
	*/
	struct OverlayCategorySort
	{
		/**
		 *\~english
		 *\brief		Comparison operator.
		 *\~french
		 *\brief		Opérateur de comparaison.
		 */
		inline bool operator()( castor3d::OverlayCategorySPtr a
			, castor3d::OverlayCategorySPtr b )const
		{
			return a->getLevel() < b->getLevel()
				|| ( a->getLevel() == b->getLevel() && a->getIndex() < b->getIndex() );
		}
	};
	using OverlayCategorySet = std::set< castor3d::OverlayCategorySPtr, OverlayCategorySort >;
	/**
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	template<>
	class ResourceCacheT< castor3d::Overlay, String > final
		: public ResourceCacheBaseT< castor3d::Overlay, String >
	{
	public:
		using ElementT = castor3d::Overlay;
		using ElementKeyT = String;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT >;
		using ElementCacheTraitsT = typename ElementCacheT::ElementCacheTraitsT;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementProducerT = typename ElementCacheT::ElementProducerT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		using iterator = std::map< ElementT, ElementKeyT >::iterator;
		using const_iterator = std::map< ElementT, ElementKeyT >::const_iterator;
		CU_DeclareMap( ElementKeyT, castor3d::FontTextureSPtr, FontTextureStr );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	merge		The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		C3D_API ResourceCacheT( castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ResourceCacheT() = default;
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
		C3D_API castor3d::FontTextureSPtr createFontTexture( FontSPtr font );
		/**
		*\~english
		*\name Iteration.
		*\~french
		*\name Itération.
		**/
		/**@{*/
		inline auto begin()
		{
			return m_overlays.begin();
		}

		inline auto begin()const
		{
			return m_overlays.begin();
		}

		inline auto end()
		{
			return m_overlays.end();
		}

		inline auto end()const
		{
			return m_overlays.end();
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
		OverlayCategorySet m_overlays;
		castor3d::Viewport m_viewport;
		std::vector< int > m_overlayCountPerLevel;
		Matrix4x4f m_projection;
		FontTextureStrMap m_fontTextures;
	};
}

#endif
