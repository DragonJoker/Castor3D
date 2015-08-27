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
#ifndef ___C3D_BORDER_PANEL_OVERLAY_H___
#define ___C3D_BORDER_PANEL_OVERLAY_H___

#include "OverlayCategory.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A rectangular overlay with a border
	\remark		Uses a spceific material for the border
	\~french
	\brief		Une incrustation rectangulaire avec un bord
	\remark		Utilise un matériau spécifique pour le bord
	*/
	class C3D_API BorderPanelOverlay
		:	public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		BorderPanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		BorderPanelOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader
			:	public OverlayCategory::TextLoader
		{
		public:
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
			virtual bool operator()( Castor3D::BorderPanelOverlay const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		BorderPanelOverlay loader
		\~english
		\brief		Loader de BorderPanelOverlay
		*/
		class C3D_API BinaryParser
			:	public OverlayCategory::BinaryParser
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
			BinaryParser( Castor::Path const & p_path );
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
			virtual bool Fill( BorderPanelOverlay const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( BorderPanelOverlay & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Sets the border material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau des bords
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		void SetBorderMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Updates the overlay position and size, taking care of wanted pixel size and position
		 *\~french
		 *\brief		Met à jour la position et la tille de l'incrustation, en prenant en compte la taille en pixel et la position en pixel voulues.
		 */
		virtual void UpdatePositionAndSize();
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		inline double GetLeftBorderSize()const
		{
			return m_ptBorderSize[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the top border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord haut
		 *\return		La valeur
		 */
		inline double GetTopBorderSize()const
		{
			return m_ptBorderSize[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the right border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord droit
		 *\return		La valeur
		 */
		inline double GetRightBorderSize()const
		{
			return m_ptBorderSize[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord bas
		 *\return		La valeur
		 */
		inline double GetBottomBorderSize()const
		{
			return m_ptBorderSize[3];
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4d const & GetBorderSize()const
		{
			return m_ptBorderSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4d & GetBorderSize()
		{
			return m_ptBorderSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the border material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau des bords
		 *\return		La valeur
		 */
		inline MaterialSPtr GetBorderMaterial()const
		{
			return m_pBorderMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetLeftBorderSize( double p_fSize )
		{
			m_ptBorderSize[0] = p_fSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetTopBorderSize( double p_fSize )
		{
			m_ptBorderSize[1] = p_fSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetRightBorderSize( double p_fSize )
		{
			m_ptBorderSize[2] = p_fSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetBottomBorderSize( double p_fSize )
		{
			m_ptBorderSize[3] = p_fSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		inline void SetBorderSize( Castor::Point4d const & p_ptSize )
		{
			m_ptBorderSize = p_ptSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		inline int GetLeftBorderPixelSize()const
		{
			return m_borderSize[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the top border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord haut
		 *\return		La valeur
		 */
		inline int GetTopBorderPixelSize()const
		{
			return m_borderSize[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the right border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord droit
		 *\return		La valeur
		 */
		inline int GetRightBorderPixelSize()const
		{
			return m_borderSize[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord bas
		 *\return		La valeur
		 */
		inline int GetBottomBorderPixelSize()const
		{
			return m_borderSize[3];
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Rectangle const & GetBorderPixelSize()const
		{
			return m_borderSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Rectangle & GetBorderPixelSize()
		{
			return m_borderSize;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetLeftBorderPixelSize( int p_size )
		{
			m_borderSize[0] = p_size;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetTopBorderPixelSize( int p_size )
		{
			m_borderSize[1] = p_size;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetRightBorderPixelSize( int p_size )
		{
			m_borderSize[2] = p_size;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetBottomBorderPixelSize( int p_size )
		{
			m_borderSize[3] = p_size;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		inline void SetBorderPixelSize( Castor::Rectangle const & p_size )
		{
			m_borderSize = p_size;
			m_changed = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the border position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de la bordure
		 *\return		La valeur
		 */
		eBORDER_POSITION GetBorderPosition()const
		{
			return m_borderPosition;
		}
		/**
		 *\~english
		 *\brief		Sets the border position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit la position de la bordure
		 *\param[in]	p_position	La nouvelle valeur
		 */
		void SetBorderPosition( eBORDER_POSITION p_position )
		{
			m_borderPosition = p_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets de la bordure
		 *\return		Le tampon
		 */
		std::vector< OverlayCategory::Vertex > const & GetBorderVertexBuffer()const
		{
			return m_arrayVtxBorder;
		}
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size, in pixels
		 *\param[in]	p_size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\param[in]	p_size	La taille de l'écran
		 *\return		La taille
		 */
		Castor::Rectangle GetAbsoluteBorderSize( Castor::Size const & p_size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay borders size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue des bordures de l'incrustation
		 *\return		La taille
		 */
		Castor::Point4d GetAbsoluteBorderSize()const;
		/**
		 *\~english
		 *\brief		Sets the borders outer part UV
		 *\param[in]	val	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie extérieure de la bordure
		 *\param[in]	val	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void SetBorderOuterUV( Castor::Point4d const & val )
		{
			m_borderOuterUv = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders outer part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie extérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetBorderOuterUV()const
		{
			return m_borderOuterUv;
		}
		/**
		 *\~english
		 *\brief		Sets the borders inner part UV
		 *\param[in]	val	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de la partie intérieure de la bordure
		 *\param[in]	val	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void SetBorderInnerUV( Castor::Point4d const & val )
		{
			m_borderInnerUv = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the borders inner part UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de la partie intérieure de la bordure
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline Castor::Point4d const & GetBorderInnerUV()const
		{
			return m_borderInnerUv;
		}

	protected:
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer
		 *\param[in]	p_renderer	The renderer used to draw this overlay
		 *\~french
		 *\brief		Met à jour le tampon de sommets
		 *\param[in]	p_renderer	Le renderer utilisé pour dessiner cette incrustation
		 */
		virtual void DoUpdate( OverlayRendererSPtr p_renderer );

	protected:
		//!\~english The border material	\~french Le matériau des bords
		MaterialWPtr m_pBorderMaterial;
		//!\~english The border size	\~french La taille des bords
		Castor::Point4d m_ptBorderSize;
		//!\~english The absolute size in pixels	\~french La taille absolue en pixels
		Castor::Rectangle m_borderSize;
		//!\~english The border material name	\~french Le nom du matériau des bords
		Castor::String m_strBorderMatName;
		//!\~english The border material name	\~french Le nom du matériau des bords
		eBORDER_POSITION m_borderPosition;
		//!\~english The borders vertex buffer data	\~french Les données du tampon de sommets pour la bordure
		std::vector< OverlayCategory::Vertex > m_arrayVtxBorder;
		//!\~english The UV for the outer part of the border	\~french Les UV de la partie extérieure de la bordure
		Castor::Point4d m_borderOuterUv;
		//!\~english The UV for the inner part of the border	\~french Les UV de la partie intérieure de la bordure
		Castor::Point4d m_borderInnerUv;
	};
}

#pragma warning( pop )

#endif
