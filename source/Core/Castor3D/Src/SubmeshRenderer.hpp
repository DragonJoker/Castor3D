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
#ifndef ___C3D_SUBMESH_RENDERER_H___
#define ___C3D_SUBMESH_RENDERER_H___

#include "Castor3DPrerequisites.hpp"
#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The submesh renderer
	\remark		Initialises the vertex, normals, textures buffers of a submesh, draws it
	\~french
	\brief		Le renderer de submesh
	\remark		Initialise les tampons d'un submesh, dessine celui-ci
	*/
	class C3D_API SubmeshRenderer
		:	public Renderer< Submesh, SubmeshRenderer >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[în]	p_pRenderSystem	Le système de rendu
		 */
		SubmeshRenderer( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~SubmeshRenderer();
		/**
		 *\~english
		 *\brief		Creates GPU buffers
		 *\~french
		 *\brief		Crée les tampons GPU
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Draws the submesh
		 *\param[in]	p_eMode	The render mode
		 *\param[in]	p_pass	The Pass containing material informations
		 *\~french
		 *\brief		Dessine le submesh
		 *\param[in]	p_eMode	Le mode de rendu
		 *\param[in]	p_pass	La Pass contenant les informations de matériau
		 */
		void Draw( eTOPOLOGY p_eMode, Pass const & p_pass );
		/**
		 *\~english
		 *\brief		Creates buffers
		 *\~french
		 *\brief		Crée les buffers
		 */
		void CreateBuffers();
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		\p true if initialised
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		\p true si initialisé
		 */
		inline bool	IsInitialised()const
		{
			return m_bInitialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the IndexBuffer of wanted primitive type
		 *\return		The IndexBuffer
		 *\~french
		 *\brief		Récupère l'IndexBuffer pour le type de primitive voulu
		 *\return		Le IndexBuffer
		 */
		inline GeometryBuffersSPtr GetGeometryBuffers()const
		{
			return m_pGeometryBuffers;
		}

	protected:
		/**
		 *\~english
		 *\brief		Draw the buffers
		 *\~french
		 *\brief		Dessine les tampons
		 */
		virtual bool DoPrepareBuffers( Pass const & p_pass );

	protected:
		//!\~english Pointer over geometry buffers	\~french Pointeur sur les buffers de la géométrie
		GeometryBuffersSPtr	m_pGeometryBuffers;
		//!\~english The actual draw type	\~french Le type de dessin actuel
		eTOPOLOGY m_eCurDrawType;
		//!\~english The previous call draw type	\~french Le type de dessin lors du précédent appel
		eTOPOLOGY m_ePrvDrawType;
		//!\~english Tells the renderer has been initialised	\~french Dit que le renderer a été initialisé
		bool m_bInitialised;
		//!\~english Tells the VAO needs reininitialisation	\~french Dit que le VAO a besoin d'être réinitialisé
		bool m_bDirty;
	};
}

#pragma warning( pop )

#endif
