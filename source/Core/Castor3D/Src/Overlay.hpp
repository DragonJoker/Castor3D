/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_OVERLAY_H___
#define ___C3D_OVERLAY_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"
#include "OverlayCategory.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		The overlay class
	\remark		An overlay is a 2D element which is displayed at foreground
	\~french
	\brief		La classe d'incrustation
	\remark		Une incrustation est un élément 2D qui est affiché en premier plan
	*/
	class Overlay
		: public std::enable_shared_from_this< Overlay >
		, public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		Overlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		Overlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextLoader
			: public Castor::Loader< Overlay, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donné dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API virtual bool operator()( Overlay const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		Overlay loader
		\~english
		\brief		Loader de Overlay
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< Overlay >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path		The current folder path
			 *\param[in]	p_engine	The engine
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path		Le chemin d'accès au dossier courant
			 *\param[in]	p_engine	Le moteur
			 */
			C3D_API BinaryParser( Castor::Path const & p_path, Engine * p_engine );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( Overlay const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( Overlay & p_obj, BinaryChunk & p_chunk )const;

			//!\~english The engine	\~french Le moteur
			Engine * m_engine;
		};

	public:
		typedef OverlayPtrArray::iterator iterator;
		typedef OverlayPtrArray::const_iterator const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_type		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_type		Le type de l'incrustation
		 */
		C3D_API Overlay( Engine & p_engine, eOVERLAY_TYPE p_type );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_type		The overlay type
		 *\param[in]	p_scene	The scene holding the overlay
		 *\param[in]	p_parent	The parent overlay (if any)
		 *\~english
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_type		Le type de l'incrustation
		 *\param[in]	p_scene	La scène parent
		 *\param[in]	p_parent	L'incrustation parente
		 */
		C3D_API Overlay( Engine & p_engine, eOVERLAY_TYPE p_type, SceneSPtr p_scene, OverlaySPtr p_parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Overlay();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_size	The render target size
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_size	Les dimensions de la cible du rendu
		 */
		C3D_API virtual void Render( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Adds a child to the overlay
		 *\param[in]	p_overlay	The overlay to add
		 *\return		\p true if successful, false if not
		 *\~french
		 *\brief		Ajoute un enfant à l'incrustation
		 *\param[in]	p_overlay	L'incrustation enfant
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool AddChild( OverlaySPtr p_overlay );
		/**
		 *\~english
		 *\brief		Retrieves the childs count at given level
		 *\param[in]	p_level	The wanted level
		 *\return		The count
		 *\~french
		 *\brief		Récupère le compte des enfants du niveau donné
		 *\param[in]	p_level	Le niveau voulu
		 *\return		Le compte
		 */
		C3D_API int GetChildsCount( int p_level )const;
		/**
		 *\~english
		 *\brief		Retrieves the panel overlay.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation panneau.
		 *\return		La catégorie.
		 */
		C3D_API PanelOverlaySPtr GetPanelOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the border panel overlay.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation panneau borduré.
		 *\return		La catégorie.
		 */
		C3D_API BorderPanelOverlaySPtr GetBorderPanelOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the text overlay
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la l'incrustation texte
		 *\return		La catégorie.
		 */
		C3D_API TextOverlaySPtr GetTextOverlay()const;
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		C3D_API bool IsVisible()const;
		/**
		 *\~english
		 *\brief		Retrieves the overlay category.
		 *\return		The category.
		 *\~french
		 *\brief		Récupère la categorie d'incrustation.
		 *\return		La catégorie.
		 */
		OverlayCategorySPtr GetCategory()const
		{
			return m_category;
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation, en pixels
		 *\param[in]	p_size	La taille de l'écran
		 *\return		La position
		 */
		Castor::Position GetAbsolutePosition( Castor::Size const & p_size )const
		{
			return m_category->GetAbsolutePosition( p_size );
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation, en pixels
		 *\param[in]	p_size	La taille de l'écran
		 *\return		La taille
		 */
		Castor::Size GetAbsoluteSize( Castor::Size const & p_size )const
		{
			return m_category->GetAbsoluteSize( p_size );
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation
		 *\return		La position
		 */
		Castor::Point2d GetAbsolutePosition()const
		{
			return m_category->GetAbsolutePosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation
		 *\return		La taille
		 */
		Castor::Point2d GetAbsoluteSize()const
		{
			return m_category->GetAbsoluteSize();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' size has changed.
		 *\~french
		 *\return		\p true si les dimensions de cette incrustation ou d'un de ses parents ont changé.
		 */
		bool IsSizeChanged()const
		{
			return m_category->IsSizeChanged();
		}
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' position has changed.
		 *\~french
		 *\return		\p true si la position de cette incrustation ou d'un de ses parents a changé.
		 */
		bool IsPositionChanged()const
		{
			return m_category->IsPositionChanged();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const
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
		inline Castor::Point2d const & GetPosition()const
		{
			return m_category->GetPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2d const & GetSize()const
		{
			return m_category->GetSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Position const & GetPixelPosition()const
		{
			return m_category->GetPixelPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Size const & GetPixelSize()const
		{
			return m_category->GetPixelSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de l'incrustation
		 *\return		La valeur
		 */
		inline eOVERLAY_TYPE GetType()const
		{
			return m_category->GetType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation parente
		 *\return		La valeur
		 */
		inline OverlaySPtr GetParent()const
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
		inline MaterialSPtr GetMaterial()const
		{
			return m_category->GetMaterial();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2d & GetPosition()
		{
			return m_category->GetPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2d & GetSize()
		{
			return m_category->GetSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Position & GetPixelPosition()
		{
			return m_category->GetPixelPosition();
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Size & GetPixelSize()
		{
			return m_category->GetPixelSize();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent scene
		 *\return		The value
		 *\~french
		 *\brief		Récupère la scène parente
		 *\return		La valeur
		 */
		inline SceneSPtr GetScene()const
		{
			return m_pScene.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'indice
		 *\return		La valeur
		 */
		inline int GetIndex()const
		{
			return m_category->GetIndex();
		}
		/**
		 *\~english
		 *\brief		Retrieves the level
		 *\return		The value
		 *\~french
		 *\brief		Récupère le niveau
		 *\return		La valeur
		 */
		inline int GetLevel()const
		{
			return m_category->GetLevel();
		}
		/**
		 *\~english
		 *\brief		Retrieves the childs count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'enfants
		 *\return		La valeur
		 */
		inline uint32_t GetChildsCount()const
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
		 *\brief		Retrieves the overlay manager
		 *\return		The value
		 *\~french
		 *\brief		Récupère le gestionnaire d'incrustations
		 *\return		La valeur
		 */
		inline OverlayManager const & GetOverlayManager()const
		{
			return m_manager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay manager
		 *\return		The value
		 *\~french
		 *\brief		Récupère le gestionnaire d'incrustations
		 *\return		La valeur
		 */
		inline OverlayManager & GetOverlayManager()
		{
			return m_manager;
		}
		/**
		 *\~english
		 *\brief		Sets the visibility status
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetVisible( bool val )
		{
			m_category->SetVisible( val );
		}
		/**
		 *\~english
		 *\brief		Sets the overlay order
		 *\param[in]	p_index	The new index
		 *\param[in]	p_level	The new level
		 *\~french
		 *\brief		Définit l'ordre de l'incrustation
		 *\param[in]	p_index	Le nouvel indice
		 *\param[in]	p_level	Le nouveau niveau
		 */
		inline void SetOrder( int p_index, int p_level )
		{
			m_category->SetOrder( p_index, p_level );
		}
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		inline void SetMaterial( MaterialSPtr p_pMaterial )
		{
			m_category->SetMaterial( p_pMaterial );
		}
		/**
		 *\~english
		 *\brief		Sets the relative position of the overlay
		 *\param[in]	p_ptPosition	The new position
		 *\~french
		 *\brief		Définit la position relative de l'incrustation
		 *\param[in]	p_ptPosition	La nouvelle position
		 */
		inline void SetPosition( Castor::Point2d const & p_ptPosition )
		{
			m_category->SetPosition( p_ptPosition );
		}
		/**
		 *\~english
		 *\brief		Sets the relative size of the overlay
		 *\param[in]	p_ptSize	The new size
		 *\~french
		 *\brief		Définit les dimensions relatives de l'incrustation
		 *\param[in]	p_ptSize	Les nouvelles dimensions
		 */
		inline void SetSize( Castor::Point2d const & p_ptSize )
		{
			m_category->SetSize( p_ptSize );
		}
		/**
		 *\~english
		 *\brief		Sets the relative position of the overlay
		 *\param[in]	p_position	The new position
		 *\~french
		 *\brief		Définit la position relative de l'incrustation
		 *\param[in]	p_position	La nouvelle position
		 */
		inline void SetPixelPosition( Castor::Position const & p_position )
		{
			m_category->SetPixelPosition( p_position );
		}
		/**
		 *\~english
		 *\brief		Sets the relative size of the overlay
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Définit les dimensions relatives de l'incrustation
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		inline void SetPixelSize( Castor::Size const & p_size )
		{
			m_category->SetPixelSize( p_size );
		}
		/**
		 *\~english
		 *\brief		Sets the name of the overlay
		 *\param[in]	p_name	The new name
		 *\~french
		 *\brief		Définit le nom de l'incrustation
		 *\param[in]	p_name	Le nouveau nom
		 */
		inline void SetName( Castor::String const & p_name )
		{
			m_name = p_name;
		}

	protected:
		//!\~english The overlay manager	\~french Le gestionnaire d'inscrustations
		OverlayManager & m_manager;
		//!\~english The overlay name	\~french Le nom de l'incrustation
		Castor::String m_name;
		//!\~english The parent overlay, if any	\~french L'incrustation parente, s'il y en a
		OverlayWPtr m_parent;
		//!\~english The childs	\~french Les enfants
		OverlayPtrArray m_overlays;
		//!\~english The overlay category	\~french La catégorie de l'incrustation
		OverlayCategorySPtr m_category;
		//!\~english The parent scene	\~french La scène parente
		SceneWPtr m_pScene;
		//!\~english The render system	\~french Le système de rendu
		RenderSystem * m_renderSystem;
	};
}

#endif
