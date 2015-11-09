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
			inline Castor::String ToString( Expr const & p_value )
			{
				return Castor::String( p_value.m_value.str() );
			}

			inline Castor::String ToString( Type const & p_value )
			{
				return Castor::String( p_value );
			}

			inline Castor::String ToString( int const & p_value )
			{
				return Castor::string::to_string( p_value );
			}

			inline Castor::String ToString( double const & p_value )
			{
				return Castor::string::to_string( p_value );
			}

			inline Castor::String ToString( float const & p_value )
			{
				return Castor::string::to_string( p_value );
			}

			//***********************************************************************************************

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params );

			template< typename Return >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return )
			{
				p_return.m_value << cuT( "()" );
			}

			template< typename Return, typename Param >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param && p_last )
			{
				p_return.m_value << p_separator << ToString( p_last ) << cuT( " )" );
			}

			template< typename Return, typename Param, typename ... Params >
			inline void WriteFunctionCallRec( Castor::String & p_separator, Return & p_return, Param && p_current, Params && ... p_params )
			{
				p_return.m_value << p_separator << ToString( p_current );
				p_separator = cuT( ", " );
				WriteFunctionCallRec( p_separator, p_return, std::forward< Params >( p_params )... );
			}

			template< typename Return, typename ... Params >
			inline Return WriteFunctionCall( GlslWriter * p_writer, Castor::String const & p_name, Params && ... p_params )
			{
				Return l_return( p_writer );
				l_return.m_value << p_name;
				Castor::String l_separator = cuT( "( " );
				WriteFunctionCallRec( l_separator, l_return, std::forward< Params >( p_params )... );
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
		inline GlslBool & GlslBool::operator=( T const & p_rhs )
		{
			UpdateWriter( p_rhs );
			m_writer->WriteAssign( *this, p_rhs );
			return *this;
		}

		template< typename T >
		inline GlslBool & GlslBool::operator=( int p_rhs )
		{
			m_writer->WriteAssign( *this, p_rhs );
			return *this;
		}

		//***********************************************************************************************

		template< typename T >
		Array< T >::Array( GlslWriter * p_writer, const Castor::String & p_name, uint32_t p_dimension )
			: T( p_writer, p_name )
			, m_dimension( p_dimension )
		{
		}

		template< typename T >
		template< typename U >
		T & Array< T >::operator[]( U const & p_offset )
		{
			T::m_value << Castor::String( *static_cast< T * >( this )  ) << cuT( "[" ) << ToString( p_offset ) << cuT( "]" );
			return *this;
		}

		//***********************************************************************************************

		template< typename TypeA, typename TypeB, typename Enable >
		TypeA operator+( TypeA const & p_a, TypeB const & p_b )
		{
			TypeA l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " + " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		TypeA operator-( TypeA const & p_a, TypeB const & p_b )
		{
			TypeA l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " - " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		TypeA operator*( TypeA const & p_a, TypeB const & p_b )
		{
			TypeA l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		TypeA operator/( TypeA const & p_a, TypeB const & p_b )
		{
			TypeA l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " / " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator==( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " == " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator!=( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " != " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator<( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " < " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator<=( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " <= " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator>( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " > " ) << ToString( p_b );
			return l_return;
		}

		template< typename TypeA, typename TypeB, typename Enable >
		GlslBool operator>=( TypeA const & p_a, TypeB const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " >= " ) << ToString( p_b );
			return l_return;
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

			if( m_keywords->HasNamedFragData() )
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

		//***********************************************************************************************

		template< typename ... Values >
		inline Float dot( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "dot" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Float inversesqrt( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "inversesqrt" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Float sqrt( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "sqrt" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Float pow( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "pow" ), p_value, p_values... );
		}

		template< typename Value, typename ... Values >
		inline Value cross( Value const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "cross" ), p_value, p_values... );
		}

		template< typename Value, typename ... Values >
		inline Value clamp( Value const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "clamp" ), p_value, p_values... );
		}

		template< typename Value, typename ... Values >
		inline Value min( Value const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "min" ), p_value, p_values... );
		}

		template< typename Value, typename ... Values >
		inline Value max( Value const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "max" ), p_value, p_values... );
		}

		template< typename Value >
		Value normalize( Value const & p_value )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "normalize" ), p_value );
		}

		template< typename Value >
		inline Value transpose( Value const & p_value )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "transpose" ), p_value );
		}

		template< typename Value >
		inline Value inverse( Value const & p_value )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "inverse" ), p_value );
		}

		template< typename Value >
		inline Value neg( Value const & p_value )
		{
			return WriteFunctionCall< Value >( p_value.m_writer, cuT( "-" ), p_value );
		}

		//***********************************************************************************************

		template< typename ... Values >
		inline Vec2 vec2( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Vec2, Values... >( p_value.m_writer, cuT( "vec2" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Vec3 vec3( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Vec3 >( p_value.m_writer, cuT( "vec3" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Vec4 vec4( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Vec4 >( p_value.m_writer, cuT( "vec4" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline IVec2 ivec2( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< IVec2 >( p_value.m_writer, cuT( "ivec2" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline IVec3 ivec3( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< IVec3 >( p_value.m_writer, cuT( "ivec3" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline IVec4 ivec4( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< IVec4 >( p_value.m_writer, cuT( "ivec4" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Mat3 mat3( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Mat3 >( p_value.m_writer, cuT( "mat3" ), p_value, p_values... );
		}

		template< typename ... Values >
		inline Mat4 mat4( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Mat4 >( p_value.m_writer, cuT( "mat4" ), p_value, p_values... );
		}

		//*****************************************************************************************

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_Light( GlslWriter & p_writer )
		{
			Struct l_lightDecl = p_writer.GetStruct( cuT( "Light" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Ambient" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Diffuse" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Specular" ) );
			l_lightDecl.GetMember< Vec4 >( cuT( "m_v4Position" ) );
			l_lightDecl.GetMember< Int >( cuT( "m_iType" ) );
			l_lightDecl.GetMember< Vec3 >( cuT( "m_v3Attenuation" ) );
			l_lightDecl.GetMember< Mat4 >( cuT( "m_mtx4Orientation" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fExponent" ) );
			l_lightDecl.GetMember< Float >( cuT( "m_fCutOff" ) );
			l_lightDecl.End();
		}

		namespace
		{
			Light GetLight( Int p_iIndex )
			{
				BUILTIN( *p_iIndex.m_writer, Sampler1D, c3d_sLights );
				LOCALE( *p_iIndex.m_writer, Light, l_lightReturn );

				if ( p_iIndex.m_writer->HasTexelFetch() )
				{
					LOCALE_ASSIGN( *p_iIndex.m_writer, Int, l_offset, p_iIndex * Int( 10 ) );
					l_lightReturn.m_v4Ambient() = texelFetch( c3d_sLights, l_offset++, 0 );
					l_lightReturn.m_v4Diffuse() = vec4( Float( p_iIndex.m_writer, 1.0f ), 1.0, 1.0, 1.0 );// texelFetch( c3d_sLights, l_offset++, 0 );
					l_lightReturn.m_v4Specular() = vec4( Float( p_iIndex.m_writer, 1.0f ), 1.0, 1.0, 1.0 );// texelFetch( c3d_sLights, l_offset++, 0 );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texelFetch( c3d_sLights, l_offset++, 0 ) );
					l_lightReturn.m_v3Attenuation() = texelFetch( c3d_sLights, l_offset++, 0 ).xyz();
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texelFetch( c3d_sLights, l_offset++, 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texelFetch( c3d_sLights, l_offset++, 0 ).xy() );
					l_lightReturn.m_v4Position() = vec4( Float( p_iIndex.m_writer, 0.0f ), -1.0, -1.0, 0.0 );// vec4( l_v4Position.z(), l_v4Position.y(), l_v4Position.x(), 0.0 );
					l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.w() );
					l_lightReturn.m_mtx4Orientation() = mat4( l_v4A, l_v4B, l_v4C, l_v4D );
					l_lightReturn.m_fExponent() = 50.0f;// l_v2Spot.x();
					l_lightReturn.m_fCutOff() = l_v2Spot.y();
				}
				else
				{
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fFactor, p_iIndex * Float( 0.01f ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fOffset, Float( 0 ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fDecal, Float( 0.0005f ) );
					LOCALE_ASSIGN( *p_iIndex.m_writer, Float, l_fMult, Float( 0.001f ) );
					l_lightReturn.m_v4Ambient() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					l_lightReturn.m_v4Diffuse() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					l_lightReturn.m_v4Specular() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4Position, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					l_lightReturn.m_v3Attenuation() = texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).xyz();
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4A, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4B, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4C, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec4, l_v4D, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ) );
					l_fOffset += l_fMult;
					LOCALE_ASSIGN( *p_iIndex.m_writer, Vec2, l_v2Spot, texture1D( c3d_sLights, l_fFactor + l_fOffset + l_fDecal ).xy() );
					l_lightReturn.m_v4Position() = vec4( l_v4Position.z(), l_v4Position.y(), l_v4Position.x(), 0.0 );
					l_lightReturn.m_iType() = CAST( *p_iIndex.m_writer, Int, l_v4Position.w() );
					l_lightReturn.m_mtx4Orientation() = mat4( l_v4A, l_v4B, l_v4C, l_v4D );
					l_lightReturn.m_fExponent() = l_v2Spot.x();
					l_lightReturn.m_fCutOff() = l_v2Spot.y();
				}

				p_iIndex.m_writer->Return( l_lightReturn );
				return l_lightReturn;
			}
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_GetLight( GlslWriter & p_writer )
		{
			p_writer.ImplementFunction< Light >( cuT( "GetLight" ), &GLSL::GetLight, Int( &p_writer, cuT( "p_iIndex" ) ) );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_ComputeLightDirection( GlslWriter & p_writer )
		{
			LightingModel::Declare_ComputeLightDirection( p_writer );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_Bump( GlslWriter & p_writer )
		{
			LightingModel::Declare_Bump( p_writer );
		}

		template< typename LightingModel >
		void Lighting< LightingModel >::Declare_ComputeFresnel( GlslWriter & p_writer )
		{
			LightingModel::Declare_ComputeFresnel( p_writer );
		}

		template< typename LightingModel >
		inline Light Lighting< LightingModel >::GetLight( Type const & p_value )
		{
			return WriteFunctionCall< Light >( p_value.m_writer, cuT( "GetLight" ), p_value );
		}

		template< typename LightingModel >
		template< typename ... Values >
		inline Vec4 Lighting< LightingModel >::ComputeLightDirection( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Vec4 >( p_value.m_writer, cuT( "ComputeLightDirection" ), p_value, p_values... );
		}

		template< typename LightingModel >
		template< typename ... Values >
		inline void Lighting< LightingModel >::Bump( Type const & p_value, Values const & ... p_values )
		{
			WriteFunctionCall< Void >( p_value.m_writer, cuT( "Bump" ), p_value, p_values... );
		}

		template< typename LightingModel >
		template< typename ... Values >
		inline Float Lighting< LightingModel >::ComputeFresnel( Type const & p_value, Values const & ... p_values )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "ComputeFresnel" ), p_value, p_values... );
		}
	}
}
