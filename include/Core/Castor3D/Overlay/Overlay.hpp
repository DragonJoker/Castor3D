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
	class Overlay
		: public std::enable_shared_from_this< Overlay >
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		Overlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		Overlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextWriter
			: public castor::TextWriter< Overlay >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	file		the file to save the overlay in
			 *\param[in]	overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donné dans un fichier texte
			 *\param[in]	file		Le fichier où enregistrer l'incrustation
			 *\param[in]	overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API bool operator()( Overlay const & overlay, castor::TextFile & file )override;
		};

	public:
		using iterator = OverlayPtrArray::iterator;
		using const_iterator = OverlayPtrArray::const_iterator;

	public:
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
			, OverlayType type );
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
			, SceneSPtr scene
			, OverlaySPtr parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Overlay();
		/**
		 *\~english
		 *\brief		adds a child to the overlay
		 *\param[in]	overlay	The overlay to add
		 *\return		\p true if successful, false if not
		 *\~french
		 *\brief		Ajoute un enfant à l'incrustation
		 *\param[in]	overlay	L'incrustation enfant
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API void addChild( OverlaySPtr overlay );
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
		C3D_API uint32_t getChildrenCount( int level )const;
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
		 *\brief		Retrieves the overlay category.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la categorie d'incrustation.
		 *\return		La catégorie.
		 */
		OverlayCategorySPtr getCategory()const
		{
			return m_category;
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position, in pixels
		 *\param[in]	size	The screen size
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation, en pixels
		 *\param[in]	size	La taille de l'écran
		 *\return		La position
		 */
		castor::Position getAbsolutePosition( castor::Size const & size )const
		{
			return m_category->getAbsolutePosition( size );
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size, in pixels
		 *\param[in]	size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation, en pixels
		 *\param[in]	size	La taille de l'écran
		 *\return		La taille
		 */
		castor::Size getAbsoluteSize( castor::Size const & size )const
		{
			return m_category->getAbsoluteSize( size );
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation
		 *\return		La position
		 */
		castor::Point2d getAbsolutePosition()const
		{
			return m_category->getAbsolutePosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation
		 *\return		La taille
		 */
		castor::Point2d getAbsoluteSize()const
		{
			return m_category->getAbsoluteSize();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' size has changed.
		 *\~french
		 *\return		\p true si les dimensions de cette incrustation ou d'un de ses parents ont changé.
		 */
		bool isSizeChanged()const
		{
			return m_category->isSizeChanged();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' position has changed.
		 *\~french
		 *\return		\p true si la position de cette incrustation ou d'un de ses parents a changé.
		 */
		bool isPositionChanged()const
		{
			return m_category->isPositionChanged();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		inline castor::String const & getName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d const & getPosition()const
		{
			return m_category->getPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d const & getSize()const
		{
			return m_category->getSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Position const & getPixelPosition()const
		{
			return m_category->getPixelPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Size const & getPixelSize()const
		{
			return m_category->getPixelSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de l'incrustation
		 *\return		La valeur
		 */
		inline OverlayType getType()const
		{
			return m_category->getType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation parente
		 *\return		La valeur
		 */
		inline OverlaySPtr getParent()const
		{
			return m_parent.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau
		 *\return		La valeur
		 */
		inline MaterialSPtr getMaterial()const
		{
			return m_category->getMaterial();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d & getPosition()
		{
			return m_category->getPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d & getSize()
		{
			return m_category->getSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Position & getPixelPosition()
		{
			return m_category->getPixelPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Size & getPixelSize()
		{
			return m_category->getPixelSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent scene
		 *\return		The value
		 *\~french
		 *\brief		Récupère la scène parente
		 *\return		La valeur
		 */
		inline SceneSPtr getScene()const
		{
			return m_scene.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'indice
		 *\return		La valeur
		 */
		inline int getIndex()const
		{
			return m_category->getIndex();
		}
		/**
		 *\~english
		 *\brief		Retrieves the level
		 *\return		The value
		 *\~french
		 *\brief		Récupère le niveau
		 *\return		La valeur
		 */
		inline int getLevel()const
		{
			return m_category->getLevel();
		}
		/**
		 *\~english
		 *\brief		Retrieves the childs count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'enfants
		 *\return		La valeur
		 */
		inline uint32_t getChildrenCount()const
		{
			return uint32_t( m_overlays.size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline iterator begin()
		{
			return m_overlays.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline const_iterator begin()const
		{
			return m_overlays.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier enfant
		 *\return		La valeur
		 */
		inline iterator end()
		{
			return m_overlays.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier enfant
		 *\return		La valeur
		 */
		inline const_iterator end()const
		{
			return m_overlays.end();
		}
		/**
		 *\~english
		 *\brief		Sets the visibility status
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void setVisible( bool val )
		{
			m_category->setVisible( val );
		}
		/**
		 *\~english
		 *\brief		Sets the overlay order
		 *\param[in]	index	The new index
		 *\param[in]	level	The new level
		 *\~french
		 *\brief		Définit l'ordre de l'incrustation
		 *\param[in]	index	Le nouvel indice
		 *\param[in]	level	Le nouveau niveau
		 */
		inline void setOrder( int index, int level )
		{
			m_category->setOrder( index, level );
		}
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	material	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	material	La nouvelle valeur
		 */
		inline void setMaterial( MaterialSPtr material )
		{
			m_category->setMaterial( material );
		}
		/**
		 *\~english
		 *\brief		Sets the relative position of the overlay
		 *\param[in]	position	The new position
		 *\~french
		 *\brief		Définit la position relative de l'incrustation
		 *\param[in]	position	La nouvelle position
		 */
		inline void setPosition( castor::Point2d const & position )
		{
			m_category->setPosition( position );
		}
		/**
		 *\~english
		 *\brief		Sets the relative size of the overlay
		 *\param[in]	size	The new size
		 *\~french
		 *\brief		Définit les dimensions relatives de l'incrustation
		 *\param[in]	size	Les nouvelles dimensions
		 */
		inline void setSize( castor::Point2d const & size )
		{
			m_category->setSize( size );
		}
		/**
		 *\~english
		 *\brief		Sets the relative position of the overlay
		 *\param[in]	position	The new position
		 *\~french
		 *\brief		Définit la position relative de l'incrustation
		 *\param[in]	position	La nouvelle position
		 */
		inline void setPixelPosition( castor::Position const & position )
		{
			m_category->setPixelPosition( position );
		}
		/**
		 *\~english
		 *\brief		Sets the relative size of the overlay
		 *\param[in]	size	The new size
		 *\~french
		 *\brief		Définit les dimensions relatives de l'incrustation
		 *\param[in]	size	Les nouvelles dimensions
		 */
		inline void setPixelSize( castor::Size const & size )
		{
			m_category->setPixelSize( size );
		}
		/**
		 *\~english
		 *\brief		Sets the name of the overlay
		 *\param[in]	name	The new name
		 *\~french
		 *\brief		Définit le nom de l'incrustation
		 *\param[in]	name	Le nouveau nom
		 */
		inline void setName( castor::String const & name )
		{
			m_name = name;
		}

	protected:
		//!\~english	The overlay name.
		//!\~french		Le nom de l'incrustation.
		castor::String m_name;
		//!\~english	The parent overlay, if any.
		//!\~french		L'incrustation parente, s'il y en a.
		OverlayWPtr m_parent;
		//!\~english	The children.
		//!\~french		Les enfants.
		OverlayPtrArray m_overlays;
		//!\~english	The overlay category.
		//!\~french		La catégorie de l'incrustation.
		OverlayCategorySPtr m_category;
		//!\~english	The parent scene.
		//!\~french		La scène parente.
		SceneWPtr m_scene;
		//!\~english	The render system.
		//!\~french		Le système de rendu.
		RenderSystem * m_renderSystem;
	};
}

#endif
