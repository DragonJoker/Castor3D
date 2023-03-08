/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Overlay_H___
#define ___C3D_Overlay_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class Overlay final
		: public castor::OwnedBy< Engine >
	{
	public:
		using iterator = OverlayPtrArray::iterator;
		using const_iterator = OverlayPtrArray::const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The overlay name.
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The overlay type.
		 *\param[in]	scene	The scene holding the overlay.
		 *\param[in]	parent	The parent overlay (if any).
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom de l'incrustation.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de l'incrustation.
		 *\param[in]	scene	La scène parent.
		 *\param[in]	parent	L'incrustation parente.
		 */
		C3D_API Overlay( castor::String const & name
			, Engine & engine
			, OverlayType type
			, Scene * scene
			, OverlayRPtr parent
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	type	The overlay type
		 *\param[in]	scene	The scene holding the overlay
		 *\param[in]	parent	The parent overlay (if any)
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur
		 *\param[in]	type	Le type de l'incrustation
		 *\param[in]	scene	La scène parent
		 *\param[in]	parent	L'incrustation parente
		 */
		C3D_API Overlay( Engine & engine
			, OverlayType type
			, Scene * scene
			, OverlayRPtr parent
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The overlay type.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de l'incrustation.
		 */
		C3D_API Overlay( Engine & engine
			, OverlayType type
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\brief		adds a child to the overlay.
		 *\param[in]	overlay	The overlay to add.
		 *\~french
		 *\brief		Ajoute un enfant à l'incrustation.
		 *\param[in]	overlay	L'incrustation enfant.
		 */
		C3D_API void addChild( OverlayRPtr overlay );
		/**
		 *\~english
		 *\brief		Retrieves the childs count at given level
		 *\param[in]	level	The wanted level
		 *\return		The count
		 *\~french
		 *\brief		Récupère le compte des enfants du niveau donné
		 *\param[in]	level	Le niveau voulu
		 *\return		Le compte
		 */
		C3D_API uint32_t getChildrenCount( uint32_t level )const;
		/**
		 *\~english
		 *\brief		Retrieves the panel overlay.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation panneau.
		 *\return		La catégorie.
		 */
		C3D_API PanelOverlaySPtr getPanelOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the border panel overlay.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation panneau borduré.
		 *\return		La catégorie.
		 */
		C3D_API BorderPanelOverlaySPtr getBorderPanelOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the text overlay
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation texte
		 *\return		La catégorie.
		 */
		C3D_API TextOverlaySPtr getTextOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		C3D_API bool isVisible()const;
		/**
		 *\~english
		 *\return		The hierarchy level for this overlay
		 *\~french
		 *\return		Le niveau dans la hiérarchie de cet overlay.
		 */
		C3D_API uint32_t computeLevel()const;
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		OverlayCategorySPtr getCategory()const noexcept
		{
			return m_category;
		}

		castor::Position getAbsolutePosition( castor::Size const & size )const noexcept
		{
			return m_category->getAbsolutePosition( size );
		}

		castor::Size getAbsoluteSize( castor::Size const & size )const noexcept
		{
			return m_category->getAbsoluteSize( size );
		}

		castor::Point2d getAbsolutePosition()const noexcept
		{
			return m_category->getAbsolutePosition();
		}

		castor::Point2d getAbsoluteSize()const noexcept
		{
			return m_category->getAbsoluteSize();
		}

		bool isSizeChanged()const noexcept
		{
			return m_category->isSizeChanged();
		}

		bool isPositionChanged()const noexcept
		{
			return m_category->isPositionChanged();
		}

		castor::String const & getName()const noexcept
		{
			return m_name;
		}

		castor::Point2d const & getRelativePosition()const noexcept
		{
			return m_category->getRelativePosition();
		}

		castor::Point2d const & getRelativeSize()const noexcept
		{
			return m_category->getRelativeSize();
		}

		castor::Position const & getPixelPosition()const noexcept
		{
			return m_category->getPixelPosition();
		}

		castor::Size const & getPixelSize()const noexcept
		{
			return m_category->getPixelSize();
		}

		OverlayType getType()const noexcept
		{
			return m_category->getType();
		}

		OverlayRPtr getParent()const noexcept
		{
			return m_parent;
		}

		MaterialRPtr getMaterial()const noexcept
		{
			return m_category->getMaterial();
		}

		SceneRPtr getScene()const noexcept
		{
			return m_scene;
		}

		uint32_t getIndex()const noexcept
		{
			return m_category->getIndex();
		}

		uint32_t getLevel()const noexcept
		{
			return m_category->getLevel();
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Array access to childs.
		*\~french
		*\name
		*	Acces au tableau d'enfants.
		*/
		/**@{*/
		size_t size()const noexcept
		{
			return uint32_t( m_children.size() );
		}

		bool empty()const noexcept
		{
			return m_children.empty();
		}

		iterator begin()
		{
			return m_children.begin();
		}

		const_iterator begin()const noexcept
		{
			return m_children.begin();
		}

		iterator end()
		{
			return m_children.end();
		}

		const_iterator end()const noexcept
		{
			return m_children.end();
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		void setVisible( bool val )
		{
			m_category->setVisible( val );
		}

		void setMaterial( MaterialRPtr material )
		{
			m_category->setMaterial( material );
		}

		void setRelativePosition( castor::Point2d const & position )
		{
			m_category->setRelativePosition( position, false );
		}

		void setRelativeSize( castor::Point2d const & size )
		{
			m_category->setRelativeSize( size, false );
		}

		void setPixelPosition( castor::Position const & position )
		{
			m_category->setPixelPosition( position, false );
		}

		void setPixelSize( castor::Size const & size )
		{
			m_category->setPixelSize( size, false );
		}

		void rename( castor::String const & name )
		{
			m_name = name;
		}
		/**@}*/

	private:
		castor::String m_name;
		OverlayRPtr m_parent{};
		OverlayPtrArray m_children;
		OverlayCategorySPtr m_category;
		Scene * m_scene;
		RenderSystem * m_renderSystem;
	};
}

#endif
