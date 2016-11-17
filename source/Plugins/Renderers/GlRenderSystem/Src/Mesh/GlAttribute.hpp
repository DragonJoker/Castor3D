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
#ifndef ___GL_ATTRIBUTE_H___
#define ___GL_ATTRIBUTE_H___

#include "Mesh/GlAttributeBase.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlAttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Count >
	class GlVecAttribute
		: public GlAttributeBase
	{
	public:
		GlVecAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, uint32_t p_divisor );
		virtual ~GlVecAttribute();
		bool Bind( bool p_bNormalised = false )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlAttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	class GlMatAttribute
		: public GlAttributeBase
	{
	public:
		GlMatAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName );
		virtual ~GlMatAttribute();
		bool Bind( bool p_bNormalised = false )override;
	};
}

#include "GlAttribute.inl"

#endif
