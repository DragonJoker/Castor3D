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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Holds specific members for an overlay category
	\~french
	\brief		Contient les membres sp�cifiques � une cat�gorie d'incrustation
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
		\brief		Contient les donn�es sp�cifiques de sommet pour un Overlay
		*/
		struct Vertex
		{
			int32_t coords[3];
			real texture[2];
		};
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
		class C3D_API TextLoader
			:	public Castor::Loader< OverlayCategory, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donn�e dans un fichier texte
			 *\param[in]	p_file		Le fichier o� enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation � enregistrer
			 *\return		\p true si tout s'est bien pass�
			 */
			virtual bool operator()( OverlayCategory const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		OverlayCategory loader
		\~english
		\brief		Loader de OverlayCategory
		*/
		class C3D_API BinaryParser
			:	public Castor3D::BinaryParser< OverlayCategory >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'acc�s au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilis�e afin de remplir le chunk de donn�es sp�cifiques
			 *\param[in]	p_obj	L'objet � �crire
			 *\param[out]	p_chunk	Le chunk � remplir
			 *\return		\p false si une erreur quelconque est arriv�e
			 */
			virtual bool Fill( OverlayCategory const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilis�e afin de r�cup�rer des donn�es sp�cifiques � partir d'un chunk
			 *\param[out]	p_obj	L'objet � lire
			 *\param[in]	p_chunk	Le chunk contenant les donn�es
			 *\return		\p false si une erreur quelconque est arriv�e
			 */
			virtual bool Parse( OverlayCategory & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType		Le type de l'incrustation
		 */
		OverlayCategory( eOVERLAY_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayCategory();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 */
		void Render();
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		D�finit le mat�riau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		virtual void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le nom de l'incrustation
		 *\return		La valeur
		 */
		Castor::String const & GetOverlayName()const;
		/**
		 *\~english
		 *\brief		Updates the overlay position and size, taking care of wanted pixel size and position
		 *\~french
		 *\brief		Met � jour la position et la tille de l'incrustation, en prenant en compte la taille en pixel et la position en pixel voulues.
		 */
		virtual void UpdatePositionAndSize();
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The position
		 *\~french
		 *\brief		R�cup�re la position absolue de l'incrustation, en pixels
		 *\param[in]	p_size	La taille de l'�cran
		 *\return		La position
		 */
		Castor::Position GetAbsolutePosition( Castor::Size const & p_size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		R�cup�re la taille absolue de l'incrustation, en pixels
		 *\param[in]	p_size	La taille de l'�cran
		 *\return		La taille
		 */
		Castor::Size GetAbsoluteSize( Castor::Size const & p_size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position
		 *\return		The position
		 *\~french
		 *\brief		R�cup�re la position absolue de l'incrustation
		 *\return		La position
		 */
		Castor::Point2d GetAbsolutePosition()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size
		 *\return		The size
		 *\~french
		 *\brief		R�cup�re la taille absolue de l'incrustation
		 *\return		La taille
		 */
		Castor::Point2d GetAbsoluteSize()const;
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	p_ptPosition	The new value
		 *\~french
		 *\brief		D�finit la position de l'incrustation
		 *\param[in]	p_ptPosition	La nouvelle valeur
		 */
		void SetPosition( Castor::Point2d const & p_ptPosition )
		{
			m_ptPosition = p_ptPosition;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	p_pParent	The parent overlay
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		D�finit la taille de l'incrustation
		 *\param[in]	p_pParent	L'overlay parent
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		void SetSize( Castor::Point2d const & p_ptSize )
		{
			m_ptSize = p_ptSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la taille de l'incrustation
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
		 *\brief		R�cup�re la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2d const & GetPosition()const
		{
			return m_ptPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la taille de l'incrustation
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
		 *\brief		R�cup�re la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Position const & GetPixelPosition()const
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la taille de l'incrustation
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
		 *\brief		R�cup�re la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2d & GetPosition()
		{
			return m_ptPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la taille de l'incrustation
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
		 *\brief		R�cup�re la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Position & GetPixelPosition()
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le type de l'incrustation
		 *\return		La valeur
		 */
		inline eOVERLAY_TYPE GetType()const
		{
			return m_eType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le statut de visibilit�
		 *\return		La valeur
		 */
		inline bool IsVisible()const
		{
			return m_bVisible;
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le mat�riau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const
		{
			return m_pMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Z index
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le Z index
		 *\return		La valeur
		 */
		inline int GetZIndex()const
		{
			return m_iCurrentZIndex;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re l'incrustation
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
		 *\brief		R�cup�re l'incrustation
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
		 *\brief		D�finit l'incrustation
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
		 *\brief		D�finit le statut de visibilit�
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetVisible( bool val )
		{
			m_bVisible = val;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		D�finit la position de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPixelPosition( Castor::Position const & val )
		{
			m_position = val;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		D�finit la taille de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPixelSize( Castor::Size const & val )
		{
			m_size = val;
		}
		/**
		 *\~english
		 *\brief		Sets the Z index
		 *\param[in]	p_iZIndex	The new value
		 *\~french
		 *\brief		D�finit le Z index
		 *\param[in]	p_iZIndex	La nouvelle valeur
		 */
		inline void SetZIndex( int p_iZIndex )
		{
			m_iCurrentZIndex = p_iZIndex;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay renderer
		 *\param[in]	p_pRenderer	The new value
		 *\~french
		 *\brief		D�finit le renderer d'incrustations
		 *\param[in]	p_pRenderer	La nouvelle valeur
		 */
		inline void SetRenderer( OverlayRendererSPtr p_pRenderer )
		{
			m_pRenderer = p_pRenderer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		R�cup�re le tampon de sommets
		 *\return		Le tampon
		 */
		std::vector< OverlayCategory::Vertex > const & GetVertexBuffer()const
		{
			return m_arrayVtx;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay UV
		 *\param[in]	val	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		D�finit Les UV de l'incrustation
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
		 *\brief		R�cup�re Les UV de l'incrustation
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetUV()const
		{
			return m_uv;
		}

	protected:
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilis� pour dessiner cette incrustation
		 */
		virtual void DoRender( OverlayRendererSPtr p_renderer ) = 0;
		/**
		 *\~english
		 *\brief		Updates the vertex buffer
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Met � jour le tampon de sommets
		 *\param[in]	p_renderer	Le renderer utilis� pour dessiner cette incrustation
		 */
		virtual void DoUpdate( OverlayRendererSPtr p_renderer ) = 0;

	protected:
		//!\~english The overlay	\~french L'incrustation
		Overlay * m_pOverlay;
		//!\~english The relative position (to parent or screen)	\~french La position relative (au parent ou � l'�cran)
		Castor::Point2d m_ptPosition;
		//!\~english The relative size (to parent or screen)	\~french La taille relative (� l'�cran ou au parent)
		Castor::Point2d m_ptSize;
		//!\~english The relative position (to parent or screen), in pixels	\~french La position relative (� l'�cran ou au parent), en pixels
		Castor::Position m_position;
		//!\~english The absolute size in pixels	\~french La taille absolue en pixels
		Castor::Size m_size;
		//!\~english The visibility	\~french La visibilit�
		bool m_bVisible;
		//!\~english The material used by the overlay	\~french Le mat�riau utilis� par l'incrustation
		MaterialWPtr m_pMaterial;
		//!\~english The overlay z index	\~french Le Z index de l'overlay
		int m_iCurrentZIndex;
		//!\~english The material name	\~french Le nom du mat�riau
		Castor::String m_strMatName;
		//!\~english The overlay type	\~french Le type de l'incrustation
		eOVERLAY_TYPE m_eType;
		//!\~english The overlay renderer	\~french Le renderer d'incrustation
		OverlayRendererWPtr m_pRenderer;
		//!\~english The vertex buffer data	\~french Les donn�es du tampon de sommets
		std::vector< OverlayCategory::Vertex > m_arrayVtx;
		//!\~english Tells the overlay has changed and must be updated	\~french Dit que l'incrustation a chang� et doit �tre mise � jour
		bool m_changed;
		//!\~english The UV for the panel	\~french Les UV du panneau
		Castor::Point4d m_uv;
	};
}

#pragma warning( pop )

#endif
