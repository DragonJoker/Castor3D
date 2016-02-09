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
#ifndef ___GL_MATRIX_BUFFER_OBJECT_H___
#define ___GL_MATRIX_BUFFER_OBJECT_H___

#include "GlBuffer.hpp"

#include <Buffer.hpp>

namespace GlRender
{
	class GlMatrixBufferObject
		: public GlBuffer< real >
	{
	public:
		GlMatrixBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, HardwareBufferPtr p_buffer );
		virtual ~GlMatrixBufferObject();

		virtual bool Create();
		virtual void Destroy();
		virtual bool Initialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual bool AttachTo( Castor3D::ShaderProgramSPtr p_program );
		virtual void Cleanup();
		virtual real * Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags );
		virtual void Unlock();
		virtual bool Bind( bool p_instantiated );
		virtual void Unbind();

	protected:
		virtual bool Bind()
		{
			return false;
		}
		virtual void DoAttributeCleanup();
		virtual bool DoAttributeInitialise();
		virtual bool DoAttributeBind( bool p_instanced );
		virtual void DoAttributeUnbind();

	private:
		GlAttributeBaseSPtr m_attribute;
		uint32_t m_valid;
		GlShaderProgramWPtr m_program;
	};
}

#endif
