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
#ifndef ___GL3_SubmeshRenderer___
#define ___GL3_SubmeshRenderer___

#include "Module_GLRender.h"
#include "GL3Buffer.h"

namespace Castor3D
{
	class C3D_GL3_API GL3SubmeshRenderer : public SubmeshRenderer
	{
	protected:
		eDRAW_TYPE m_eDrawType;
		eNORMALS_MODE m_eNormalsMode;
		GLVertexArrayObjects m_vao;

	public:
		GL3SubmeshRenderer( SceneManager * p_pSceneManager);
		virtual ~GL3SubmeshRenderer();

		virtual void Cleanup();
		virtual void Initialise();
		virtual VertexInfosBufferPtr CreateVertexBuffer();

	private:
		void _createVAOs( GL3ShaderProgramPtr p_pProgram);

		virtual void _drawBuffers( const Pass & p_pPass);
	};
}

#endif