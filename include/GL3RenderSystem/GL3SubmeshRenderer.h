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
#ifndef ___Gl3_SubmeshRenderer___
#define ___Gl3_SubmeshRenderer___

#include "Module_Gl3Render.h"
#include "Gl3Buffer.h"

namespace Castor3D
{
	class C3D_Gl3_API Gl3SubmeshRenderer : public SubmeshRenderer
	{
	protected:
		ePRIMITIVE_TYPE m_eDrawType;
		eNORMALS_MODE m_eNormalsMode;
		GlVertexArrayObjects m_vao;

	public:
		Gl3SubmeshRenderer( SceneManager * p_pSceneManager);
		virtual ~Gl3SubmeshRenderer();

		virtual void Cleanup();
		virtual void Initialise();
//		virtual VertexBufferPtr CreateVertexBuffer();

	private:
		void _createVAOs( Gl3ShaderProgramPtr p_pProgram);

		virtual void _drawBuffers( const Pass & p_pPass);
	};
}

#endif