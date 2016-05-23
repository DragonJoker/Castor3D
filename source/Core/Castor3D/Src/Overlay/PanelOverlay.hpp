/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_PANEL_OVERLAY_H___
#define ___C3D_PANEL_OVERLAY_H___

#include "OverlayCategory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A simple rectangular overlay
	\~french
	\brief		Une simple incrustation rectangulaire
	*/
	class PanelOverlay
		: public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		PanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		PanelOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class TextLoader
			: public OverlayCategory::TextLoader
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
			C3D_API virtual bool operator()( PanelOverlay const & p_overlay, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API PanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		C3D_API static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Retrieves the panel vertex buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de sommets du panneau
		 *\return		Le tampon
		 */
		inline OverlayCategory::VertexArray const & GetPanelVertex()const
		{
			return m_arrayVtx;
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
		C3D_API virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	p_size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	p_size	Les dimensions de la cible de rendu.
		 */
		C3D_API virtual void DoUpdateBuffer( Castor::Size const & p_size );

	protected:
		//!\~english The vertex buffer data	\~french Les données du tampon de sommets
		VertexArray m_arrayVtx;
	};
}

#endif
