/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
	class GlAttribute
		: public GlAttributeBase
	{
	public:
		GlAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, Castor3D::BufferDeclaration const & p_declaration, Castor::String const & p_attributeName );
		virtual ~GlAttribute();
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
		GlMatAttribute( OpenGl & p_gl, Castor3D::ShaderProgram const & p_program, Castor3D::BufferDeclaration const & p_declaration, Castor::String const & p_attributeName );
		virtual ~GlMatAttribute();
		virtual bool Bind( bool p_bNormalised = false );
	};
}

#include "GlAttribute.inl"

#endif
