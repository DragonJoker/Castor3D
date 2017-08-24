/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_ATTRIBUTE_BASE_H___
#define ___GL_ATTRIBUTE_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Mesh/Buffer/Buffer.hpp>

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
		: public Holder
	{
	public:
		GlAttributeBase( OpenGl & gl
			, castor3d::ShaderProgram const & program
			, uint32_t stride
			, castor::String const & attributeName
			, GlType glType
			, uint32_t count
			, uint32_t divisor );
		virtual ~GlAttributeBase();

		virtual void bind( bool normalised = false );
		virtual void unbind();

		inline void setOffset( uint32_t offset )
		{
			m_offset = offset;
		}

		inline uint32_t getLocation()const
		{
			return m_attributeLocation;
		}

	protected:
		castor3d::ShaderProgram const & m_program;
		uint32_t m_stride;
		castor::String m_attributeName;
		uint32_t m_attributeLocation;
		uint32_t m_count;
		uint32_t m_divisor;
		uint32_t m_offset;
		GlType m_glType;
	};
}

#endif
