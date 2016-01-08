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
#ifndef ___C3D_OVERLAY_CATEGORY_H___
#define ___C3D_OVERLAY_CATEGORY_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Holds specific members for an overlay category
	\~french
	\brief		Contient les membres spécifiques à une catégorie d'incrustation
	*/
	class OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		28/11/2014
		\version	0.7.0
		\~english
		\brief		Holds specific vertex data for an Overlay
		\~french
		\brief		Contient les données spécifiques de sommet pour un Overlay
		*/
		struct Vertex
		{
			int32_t coords[2];
			float texture[2];
		};
		DECLARE_VECTOR( Vertex, Vertex );
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		OverlayCategory loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		OverlayCategory loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextLoader
			: public Castor::Loader< OverlayCategory, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
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
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			C3D_API virtual bool operator()( OverlayCategory const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		OverlayCategory loader
		\~english
		\brief		Loader de OverlayCategory
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< OverlayCategory >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );
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
			C3D_API virtual bool Fill( OverlayCategory const & p_obj, BinaryChunk & p_chunk )const;
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
			C3D_API virtual bool Parse( OverlayCategory & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type de l'incrustation
		 */
		C3D_API OverlayCategory( eOVERLAY_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~OverlayCategory();
		/**
		 *\~english
		 *\brief		Initialises the overlay.
		 *\~french
		 *\brief		Initialise l'incrustation.
		 */
		C3D_API virtual void Initialise() {}
		/**
		 *\~english
		 *\brief		Cleanus the overlay up.
		 *\~french
		 *\brief		Nettoie l'incrustation.
		 */
		C3D_API virtual void Cleanup() {}
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 */
		C3D_API void Render();
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		C3D_API virtual void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		C3D_API Castor::String const & GetOverlayName()const;
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
		C3D_API Castor::Position GetAbsolutePosition( Castor::Size const & p_size )const;
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
		C3D_API Castor::Size GetAbsoluteSize( Castor::Size const & p_size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation
		 *\return		La position
		 */
		C3D_API Castor::Point2d GetAbsolutePosition()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation
		 *\return		La taille
		 */
		C3D_API Castor::Point2d GetAbsoluteSize()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' size has changed.
		 *\~french
		 *\return		\p true si les dimensions de cette incrustation ou d'un de ses parents ont changé.
		 */
		C3D_API bool IsSizeChanged()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' position has changed.
		 *\~french
		 *\return		\p true si la position de cette incrustation ou d'un de ses parents a changé.
		 */
		C3D_API bool IsPositionChanged()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		C3D_API virtual bool IsChanged()const
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	p_ptPosition	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	p_ptPosition	La nouvelle valeur
		 */
		inline void SetPosition( Castor::Point2d const & p_ptPosition )
		{
			m_position = p_ptPosition;
			m_positionChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	p_parent	The parent overlay
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	p_parent	L'overlay parent
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		inline void SetSize( Castor::Point2d const & p_ptSize )
		{
			m_ptSize = p_ptSize;
			m_sizeChanged = true;
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
			return m_ptSize;
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
			return m_position;
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
			return m_size;
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
			return m_pxPosition;
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
			return m_ptSize;
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
			return m_position;
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
			return m_size;
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
			return m_pxPosition;
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
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		inline bool IsVisible()const
		{
			return m_visible;
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
			return m_pMaterial.lock();
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
			return m_index;
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
			return m_level;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay const & GetOverlay()const
		{
			return *m_pOverlay;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay & GetOverlay()
		{
			return *m_pOverlay;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetOverlay( Overlay * val )
		{
			m_pOverlay = val;
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
			m_visible = val;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPixelPosition( Castor::Position const & val )
		{
			m_pxPosition = val;
			m_positionChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPixelSize( Castor::Size const & val )
		{
			m_size = val;
			m_sizeChanged = true;
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
			m_index = p_index;
			m_level = p_level;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay UV
		 *\param[in]	val	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de l'incrustation
		 *\param[in]	val	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void SetUV( Castor::Point4d const & val )
		{
			m_uv = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de l'incrustation
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetUV()const
		{
			return m_uv;
		}

	protected:
		/**
		 *\~english
		 *\return		The screen or parent's size.
		 *\~french
		 *\return		La taille de l'écran ou du parent.
		 */
		Castor::Point2d DoGetTotalSize()const;
		/**
		 *\~english
		 *\brief		Updates the overlay position, taking care of wanted pixel position.
		 *\~french
		 *\brief		Met à jour la position de l'incrustation, en prenant en compte la la position en pixel voulue.
		 */
		virtual void DoUpdatePosition();
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		virtual void DoUpdateSize();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		virtual void DoRender( OverlayRendererSPtr p_renderer ) = 0;
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size	Les dimensions de la cible de rendu.
		 */
		virtual void DoUpdateBuffer( Castor::Size const & p_size ) = 0;

	protected:
		//!\~english The overlay	\~french L'incrustation
		Overlay * m_pOverlay;
		//!\~english The relative position (to parent or screen)	\~french La position relative (au parent ou à l'écran)
		Castor::Point2d m_position;
		//!\~english The relative size (to parent or screen)	\~french La taille relative (à l'écran ou au parent)
		Castor::Point2d m_ptSize;
		//!\~english The relative position (to parent or screen), in pixels	\~french La position relative (à l'écran ou au parent), en pixels
		Castor::Position m_pxPosition;
		//!\~english The absolute size in pixels	\~french La taille absolue en pixels
		Castor::Size m_size;
		//!\~english The visibility	\~french La visibilité
		bool m_visible;
		//!\~english The material used by the overlay	\~french Le matériau utilisé par l'incrustation
		MaterialWPtr m_pMaterial;
		//!\~english The overlay index	\~french L'index de l'overlay
		int m_index;
		//!\~english The overlay level	\~french Le niveau de l'overlay
		int m_level;
		//!\~english The material name	\~french Le nom du matériau
		Castor::String m_strMatName;
		//!\~english The overlay type	\~french Le type de l'incrustation
		eOVERLAY_TYPE m_type;
		//!\~english The vertex buffer data	\~french Les données du tampon de sommets
		VertexArray m_arrayVtx;
		//!\~english Tells if this overlay's size has changed.	\~french Dit si les dimensions de cette incrustation ont changé.
		bool m_sizeChanged;
		//!\~english Tells if this overlay's position has changed.	\~french Dit si la position de cette incrustation a changé.
		bool m_positionChanged;
		//!\~english The UV for the panel	\~french Les UV du panneau
		Castor::Point4d m_uv;
	};
}

#endif
