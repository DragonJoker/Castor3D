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
#ifndef ___GL_TEXTURE_BUFFER_H___
#define ___GL_TEXTURE_BUFFER_H___

#include "GlBuffer.hpp"

#include <Buffer.hpp>

namespace GlRender
{
	class GlTextureBufferObject
		:	public GlBuffer< uint8_t >
		,	public Castor::NonCopyable
	{
	public:
		GlTextureBufferObject( OpenGl & p_gl, HardwareBufferPtr p_pBuffer );
		virtual ~GlTextureBufferObject();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void Cleanup();
		virtual bool Bind();
		virtual void Unbind();
		virtual uint8_t * Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void Unlock();
	};
}

#endif
