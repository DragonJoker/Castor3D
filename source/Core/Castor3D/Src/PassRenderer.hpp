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
#ifndef ___C3D_PASS_RENDERER_H___
#define ___C3D_PASS_RENDERER_H___

#include "Pass.hpp"
#include "Renderer.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	//! The Pass renderer
	/*!
	Initialises a pass, draws it, removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API PassRenderer
		:	public Renderer< Pass, PassRenderer >
	{
	public:
		/**
		 * Destructor
		 */
		virtual ~PassRenderer();
		/**
		 * Cleans up the renderer
		 */
		virtual void Cleanup();
		/**
		 * Initialises the material
		 */
		virtual void Initialise() = 0;
		/**
		 * Applies the material
		 */
		virtual void Render();
		/**
		 * Applies the material
		 */
		virtual void Render2D();
		/**
		 * Removes the material
		 */
		virtual void EndRender() = 0;

	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		PassRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Fills shader frame variables with pass values
		 *\~french
		 *\brief		Remplit les variables de shader avec les valeurs de la passe
		 */
		void DoFillShader();

	protected:
		//! The shader program
		ShaderProgramBaseWPtr m_pProgram;
		//! The ambient colour frame variable
		Point4fFrameVariableWPtr m_pAmbient;
		//! The diffuser colour frame variable
		Point4fFrameVariableWPtr m_pDiffuse;
		//! The specular colour frame variable
		Point4fFrameVariableWPtr m_pSpecular;
		//! The emissive colour frame variable
		Point4fFrameVariableWPtr m_pEmissive;
		//! The shininess value frame variable
		OneFloatFrameVariableWPtr m_pShininess;
		//! The opacity value frame variable
		OneFloatFrameVariableWPtr m_pOpacity;
		//! The camera position frame variable
		Point3rFrameVariableWPtr m_pCameraPos;
		//! The ambient colour value
		Castor::Point4f m_ptAmbient;
		//! The diffuse colour value
		Castor::Point4f m_ptDiffuse;
		//! The specular colour value
		Castor::Point4f m_ptSpecular;
		//! The emissive colour value
		Castor::Point4f m_ptEmissive;
		//! The camera position value
		Castor::Point3r m_ptCameraPos;
	};
}

#pragma warning( pop )

#endif
