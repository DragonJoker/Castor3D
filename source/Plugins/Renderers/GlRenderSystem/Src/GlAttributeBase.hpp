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
#ifndef ___GL_ATTRIBUTE_BASE_H___
#define ___GL_ATTRIBUTE_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "GlShaderProgram.hpp"

#include <Buffer.hpp>

namespace GlRender
{
	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlAttributeBase
		: public Castor::NonCopyable
		, public Holder
	{
	public:
		GlAttributeBase( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_attributeName, eGL_TYPE p_glType, int p_count );
		virtual ~GlAttributeBase();

		virtual void SetShader( Castor3D::ShaderProgramBaseSPtr p_program );
		virtual void Cleanup();
		virtual bool Initialise();
		virtual bool Bind( bool p_bNormalised = false );
		virtual void Unbind();

		inline void SetOffset( uint32_t p_offset )
		{
			m_offset = p_offset;
		}
		inline void SetStride( int p_iStride )
		{
			m_stride = p_iStride;
		}

		inline uint32_t GetLocation()const
		{
			return m_attributeLocation;
		}

	protected:
		Castor3D::ShaderProgramBaseWPtr m_program;
		Castor::String m_attributeName;
		uint32_t m_attributeLocation;
		int m_count;
		uint32_t m_offset;
		int m_stride;
		eGL_TYPE m_glType;
		GlRenderSystem * m_renderSystem;
	};
}

#endif
