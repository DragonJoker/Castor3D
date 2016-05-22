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

namespace GLSL
{
	//*****************************************************************************************

	SamplerBuffer::SamplerBuffer()
		: Type( cuT( "samplerBuffer " ) )
	{
	}

	SamplerBuffer::SamplerBuffer( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "samplerBuffer " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerBuffer & SamplerBuffer::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	SamplerBuffer::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler1D::Sampler1D()
		: Type( cuT( "sampler1D " ) )
	{
	}

	Sampler1D::Sampler1D( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "sampler1D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler1D & Sampler1D::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Sampler1D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler2D::Sampler2D()
		: Type( cuT( "sampler2D " ) )
	{
	}

	Sampler2D::Sampler2D( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "sampler2D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler2D & Sampler2D::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Sampler2D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler3D::Sampler3D()
		: Type( cuT( "sampler3D " ) )
	{
	}

	Sampler3D::Sampler3D( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "sampler3D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler3D & Sampler3D::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Sampler3D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	SamplerCube::SamplerCube()
		: Type( cuT( "samplerCube " ) )
	{
	}

	SamplerCube::SamplerCube( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "samplerCube " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerCube & SamplerCube::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	SamplerCube::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************
}
