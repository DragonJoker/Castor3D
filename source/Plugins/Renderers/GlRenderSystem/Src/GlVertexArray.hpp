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
#ifndef ___GL_VERTEX_ARRAY_H___
#define ___GL_VERTEX_ARRAY_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Buffer.hpp>

namespace GlRender
{
	class GlVertexArray
		: public Castor3D::GpuBuffer< uint8_t >
		, public Castor::NonCopyable
	{
	public:
		GlVertexArray( GlRenderSystem & p_renderSystem, OpenGl & p_gl, Castor3D::BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer );
		virtual ~GlVertexArray();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t CU_PARAM_UNUSED( p_uiOffset ), uint32_t CU_PARAM_UNUSED( p_uiCount ), uint32_t CU_PARAM_UNUSED( p_uiFlags ) )
		{
			return NULL;
		}
		virtual void Unlock() {}
		virtual bool Bind();
		virtual void Unbind();
		virtual bool Fill( uint8_t const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature )
		{
			return false;
		}

		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_pBuffer;
		}

	private:
		HardwareBufferPtr m_pBuffer;
		Castor3D::BufferDeclaration const & m_bufferDeclaration;
		OpenGl & m_gl;
	};
}

#endif
