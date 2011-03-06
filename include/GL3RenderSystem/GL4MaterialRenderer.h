/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Gl4_MaterialRenderer___
#define ___Gl4_MaterialRenderer___

#include "Module_Gl3Render.h"

namespace Castor3D
{
	class C3D_Gl3_API Gl4PassRenderer : public PassRenderer
	{
	protected:
		Point4f			m_varAmbient;
		Point4f			m_varDiffuse;
		Point4f			m_varSpecular;
		Point4f			m_varEmissive;
		float			m_varShininess;
		bool			m_bChanged;

		GlUboPoint4fVariablePtr 	m_pAmbient;
		GlUboPoint4fVariablePtr 	m_pDiffuse;
		GlUboPoint4fVariablePtr 	m_pEmissive;
		GlUboPoint4fVariablePtr 	m_pSpecular;
		GlUboFloatVariablePtr 		m_pShininess;
		GlUniformBufferObjectPtr	m_pUniformBuffer;

	public:
		Gl4PassRenderer( SceneManager * p_pSceneManager);
		virtual ~Gl4PassRenderer();

		virtual void Cleanup();
		virtual void Initialise();

		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		virtual void Render2D();
		virtual void EndRender();
	};
}

#endif