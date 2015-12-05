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
namespace GlRender
{
	namespace GLSL
	{
		gl_PerVertex::gl_PerVertex()
			: Type( cuT( "gl_PerVertex " ) )
		{
		}
		
		gl_PerVertex::gl_PerVertex( GlslWriter * p_writer, Castor::String const & p_name )
			: Type( cuT( "gl_PerVertex " ), p_writer, p_name )
		{
		}
		
		gl_PerVertex & gl_PerVertex::operator=( gl_PerVertex const & p_rhs )
		{
			if ( m_writer )
			{
				m_writer->WriteAssign( *this, p_rhs );
			}
			else
			{
				Type::operator=( p_rhs );
				m_writer = p_rhs.m_writer;
			}

			return *this;
		}

		template< typename T >
		gl_PerVertex & gl_PerVertex::operator=( T const & p_rhs )
		{
			UpdateWriter( p_rhs );
			m_writer->WriteAssign( *this, p_rhs );
			return *this;
		}

		Vec4 gl_PerVertex::gl_Position()const
		{
			return Vec4( m_writer, m_value.str() + cuT( ".gl_Position" ) );
		}
		
		Float gl_PerVertex::gl_PointSize()const
		{
			return Float( m_writer, m_value.str() + cuT( ".gl_PointSize" ) );
		}
		
		Float gl_PerVertex::gl_ClipDistance()const
		{
			return Array< Float >( m_writer, m_value.str() + cuT( ".gl_ClipDistance" ), 8 );
		}
	}
}
