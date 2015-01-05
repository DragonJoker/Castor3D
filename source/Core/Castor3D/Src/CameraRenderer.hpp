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
#ifndef ___C3D_CAMERA_RENDERER_H___
#define ___C3D_CAMERA_RENDERER_H___

#include "Camera.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The Camera renderer
	\remark		Applies the camera transformations, also removes it
	\~french
	\brief		Renderer de camera
	\remark		Applique les transformations d'une caméra, et les annule aussi
	*/
	class C3D_API CameraRenderer
		:	public Renderer<Camera, CameraRenderer>
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le RenderSystem
		 */
		CameraRenderer( RenderSystem * p_pRenderSystem );
	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~CameraRenderer();
		/**
		 *\~english
		 *\brief		Returns the first object at mouse coords x and y
		 *\param[in]	p_pScene	The scene used for the selection
		 *\param[in]	p_eMode		The selection mode (vertex, face, submesh, geometry)
		 *\param[in]	p_iX		The x mouse coordinate
		 *\param[in]	p_iY		The y mouse coordinate
		 *\param[out]	p_stFound	The selection result
		 *\return		\p true if something was found, false if not
		 *\~french
		 *\brief		Récupère l'objet le plus proche aux coordonnées souris x et y
		 *\param[in]	p_pScene	La scène où on doit sélectionner un objet
		 *\param[in]	p_eMode		Le mode de sélection (vertex, face, submesh, geometry)
		 *\param[in]	p_iX, p_iY	Les coordonnées de la souris
		 *\param[out]	p_stFound	Le résultat de la sélection
		 *\return		\p false si aucun objet n'a été trouvé
		*/
		virtual bool Select( SceneSPtr p_pScene, eSELECTION_MODE p_eMode, int p_iX, int p_iY, stSELECT_RESULT & p_stFound ) = 0;
		/**
		 *\~english
		 *\brief		Renders the target
		 *\~french
		 *\brief		Renders the target
		 */
		virtual void Render() = 0;
		/**
		 *\~english
		 *\brief		Ends the render
		 *\~french
		 *\brief		Termine le rendu
		 */
		virtual void EndRender() = 0;

	protected:
		//!\~english Tells if the camera is to be resized	\~french Dit si on doit redimensionner le viewport
		bool m_bResize;
	};
}

#endif
