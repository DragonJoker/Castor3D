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
#ifndef ___C3D_PANEL_OVERLAY_H___
#define ___C3D_PANEL_OVERLAY_H___

#include "OverlayCategory.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#ifdef DrawText
#	undef DrawText
#endif

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
	class C3D_API PanelOverlay
		:	public OverlayCategory
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
			virtual bool operator()( PanelOverlay const & p_overlay, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		PanelOverlay loader
		\~english
		\brief		Loader de PanelOverlay
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
			virtual bool Fill( PanelOverlay const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( PanelOverlay & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PanelOverlay();
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
		 *\copydoc		Castor3D::OverlayCategory::DoRender.
		 */
		virtual void DoRender( OverlayRendererSPtr p_renderer );
		/**
		 *\copydoc		Castor3D::OverlayCategory::DoUpdateBuffer.
		 */
		virtual void DoUpdateBuffer( Castor::Size const & p_screenSize );
	};
}

#pragma warning( pop )

#endif
