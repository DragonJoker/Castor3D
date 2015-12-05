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
		namespace
		{
			//***********************************************************************************************

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params );

			template< typename Return >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return )
			{
				p_return.m_value << cuT( "()" );
			}

			template< typename Return, typename Param >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_last )
			{
				p_return.m_value << p_separator << ToString( p_last ) << cuT( " )" );
			}

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param const & p_current, Params const & ... p_params )
			{
				p_return.m_value << p_separator << ToString( p_current );
				p_separator = cuT( ", " );
				WriteFunctionCallRec( p_separator, p_return, p_params... );
			}

			template< typename Return, typename ... Params >
			inline Return WriteFunctionCall( GlslWriter * p_writer, Castor::String const & p_name, Params const & ... p_params )
			{
				Return l_return( p_writer );
				l_return.m_value << p_name;
				Castor::String l_separator = cuT( "( " );
				WriteFunctionCallRec( l_separator, l_return, p_params... );
				return l_return;
			}

			//***********************************************************************************************

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params );

			template< typename Return >
			inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return )
			{
				p_return.m_value << cuT( "()" );
			}

			template< typename Return, typename Param >
			inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_last )
			{
				p_return.m_value << p_separator << p_last.m_type << p_last.m_name << cuT( " )" );
			}

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionHeaderRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params )
			{
				p_return.m_value << p_separator << p_current.m_type << p_current.m_name;
				p_separator = cuT( ", " );
				WriteFunctionHeaderRec( p_separator, p_return, std::forward< Params >( p_params )... );
			}

			template< typename Return, typename ... Params >
			inline void WriteFunctionHeader( GlslWriter * p_writer, Castor::String const & p_name, Params && ... p_params )
			{
				Return l_return( p_writer );
				l_return.m_value << l_return.m_type << cuT( " " ) << p_name;
				Castor::String l_separator = cuT( "( " );
				WriteFunctionHeaderRec( l_separator, l_return, std::forward< Params >( p_params )... );
				*p_writer << Castor::String( l_return ) << Endl();
			}

			template<>
			inline void WriteFunctionHeader< void >( GlslWriter * p_writer, Castor::String const & p_name )
			{
				Void l_return( p_writer );
				l_return.m_value << l_return.m_type << cuT( " " ) << p_name << cuT( "()" );
				*p_writer << Castor::String( l_return ) << Endl();
			}

			template< typename Return, typename ... Params >
			inline void WriteFunctionHeader( GlslWriter & p_writer, Castor::String const & p_name, Params && ... p_params )
			{
				WriteFunctionHeader< Return >( &p_writer, p_name, p_params... );
			}
		}

		//***********************************************************************************************

		template< typename T >
		void WriteAssign( GlslWriter * p_writer, Type const & p_lhs, T const & p_rhs )
		{
			p_writer->WriteAssign( p_lhs, p_rhs );
		}

		//***********************************************************************************************

		template< typename T >
		inline T Ubo::GetUniform( Castor::String const & p_name )
		{
			m_writer << Uniform() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( &m_writer, p_name );
		}

		template< typename T >
		inline Array< T > Ubo::GetUniform( Castor::String const & p_name, uint32_t p_dimension )
		{
			m_writer << Uniform() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( &m_writer, p_name, p_dimension );
		}

		//***********************************************************************************************

		template< typename T >
		inline T Struct::GetMember( Castor::String const & p_name )
		{
			m_writer << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( &m_writer, p_name );
		}

		template< typename T >
		inline Array< T > Struct::GetMember( Castor::String const & p_name, uint32_t p_dimension )
		{
			m_writer << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( &m_writer, p_name, p_dimension );
		}

		//***********************************************************************************************

		template< typename RetType, typename FuncType, typename ... Params >
		inline void GlslWriter::ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params )
		{
			WriteFunctionHeader< RetType >( *this, p_name, p_params... );
			{
				IndentBlock l_block( *this );
				p_function( std::forward < Params && > ( p_params )... );
			}
			m_stream << std::endl;
		}

		template< typename RetType >
		inline void GlslWriter::Return( RetType const & p_return )
		{
			m_stream << cuT( "return " ) << Castor::String( p_return ) << cuT( ";" ) << std::endl;
		}

		template< typename ExprType >
		ExprType GlslWriter::Paren( ExprType const p_expr )
		{
			ExprType l_return( p_expr.m_writer );
			l_return.m_value << cuT( "( " ) << Castor::String( p_expr ) << cuT( " )" );
			return l_return;
		}

		template< typename T >
		inline T GlslWriter::Cast( Type const & p_from )
		{
			T l_return;
			l_return.m_value << Castor::string::trim( l_return.m_type ) << cuT( "( " ) << Castor::String( p_from ) << cuT( " )" );
			return l_return;
		}

		template< typename T >
		inline T GlslWriter::GetAttribute( Castor::String const & p_name )
		{
			*this << Attribute() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetOut( Castor::String const & p_name )
		{
			*this << Out() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetIn( Castor::String const & p_name )
		{
			*this << In() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetLocale( Castor::String const & p_name )
		{
			*this << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );;
		}

		template< typename T >
		inline T GlslWriter::GetLocale( Castor::String const & p_name, Expr const & p_rhs )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
			T l_return( this, p_name );
			return l_return;
		}

		template< typename T >
		inline T GlslWriter::GetLocale( Castor::String const & p_name, Type const & p_rhs )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
			T l_return( this, p_name );
			return l_return;
		}

		template< typename T >
		inline T GlslWriter::GetBuiltin( Castor::String const & p_name )
		{
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetLayout( Castor::String const & p_name )
		{
			*this << Layout() << Out() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetUniform( Castor::String const & p_name )
		{
			*this << Uniform() << T().m_type << p_name << cuT( ";" ) << Endl();
			return T( this, p_name );
		}

		template< typename T >
		inline T GlslWriter::GetFragData( Castor::String const & p_name, uint32_t p_index )
		{
			Castor::String l_name;

			if ( m_keywords->HasNamedFragData() )
			{
				*this << T().m_type << p_name << cuT( ";" ) << Endl();
				l_name = p_name;
			}
			else
			{
				l_name = m_keywords->GetFragData( p_index );
			}

			return T( this, l_name );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetAttribute( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << Attribute() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetOut( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << Out() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetIn( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << Out() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, Expr const & p_rhs )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << p_rhs.m_value.rdbuf() << cuT( ";" ) << std::endl;
			return Array< T >( this, p_name );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetLocale( Castor::String const & p_name, uint32_t p_dimension, Type const & p_rhs )
		{
			m_stream << T().m_type << p_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
			return Array< T >( this, p_name );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetBuiltin( Castor::String const & p_name, uint32_t p_dimension )
		{
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetLayout( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << Layout() << Out() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}

		template< typename T >
		inline Array< T > GlslWriter::GetUniform( Castor::String const & p_name, uint32_t p_dimension )
		{
			*this << Uniform() << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( this, p_name, p_dimension );
		}
	}
}
