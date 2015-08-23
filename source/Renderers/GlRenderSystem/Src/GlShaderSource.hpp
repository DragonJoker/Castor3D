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
#ifndef ___GL_SHADER_SOURCCE_H___
#define ___GL_SHADER_SOURCCE_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <StreamIndentManipulators.hpp>

#ifdef max
#	undef max
#	undef min
#	undef abs
#endif

#ifdef OUT
#	undef OUT
#endif

#ifdef IN
#	undef IN
#endif

namespace GlRender
{
	namespace GLSL
	{
		class GlslWriter;

		template< typename T > struct is_type : public std::false_type {};

		struct Expr
		{
			Expr();
			Expr( int p_value );
			Expr( float p_value );
			Expr( double p_value );
			Expr( GlslWriter * p_writer );
			Expr( GlslWriter * p_writer, Castor::String const & p_init );
			Expr( Expr const & p_rhs );
			Expr & operator =( Expr const & p_rhs );

			GlslWriter * m_writer;
			mutable Castor::StringStream m_value;
		};

		struct Type: public Expr
		{
			Type( Castor::String const & p_type );
			Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name=Castor::String() );
			Type( Type const & p_rhs );
			Type & operator =( Type const & p_rhs );
			operator Castor::String()const;
			Type & operator+=( Type const & p_type );
			Type & operator-=( Type const & p_type );
			Type & operator*=( Type const & p_type );
			Type & operator/=( Type const & p_type );
			Type & operator+=( float p_type );
			Type & operator-=( float p_type );
			Type & operator*=( float p_type );
			Type & operator/=( float p_type );
			Type & operator+=( int p_type );
			Type & operator-=( int p_type );
			Type & operator*=( int p_type );
			Type & operator/=( int p_type );

			Castor::String m_type;
			Castor::String m_name;
		};

		struct Bool: public Type
		{
			Bool(): Type( cuT( "bool " ) ){}
			Bool( bool p_value ): Type( cuT( "bool " ) ){ m_value << p_value; }
			Bool( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "bool " ), p_writer, p_name ){}
			template< typename T > inline Bool & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Bool & operator=( int p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			inline operator uint32_t() { return 0u; }
		};

		Bool operator==( Type const & p_a, Type const & p_b );
		Bool operator!=( Type const & p_a, Type const & p_b );
		Type operator+( Type const & p_a, Type const & p_b );
		Type operator-( Type const & p_a, Type const & p_b );
		Type operator*( Type const & p_a, Type const & p_b );
		Type operator/( Type const & p_a, Type const & p_b );
		Type operator+( Type const & p_a, float p_b );
		Type operator-( Type const & p_a, float p_b );
		Type operator*( Type const & p_a, float p_b );
		Type operator/( Type const & p_a, float p_b );
		Type operator+( Type const & p_a, int p_b );
		Type operator-( Type const & p_a, int p_b );
		Type operator*( Type const & p_a, int p_b );
		Type operator/( Type const & p_a, int p_b );

		template< typename T >
		struct Array: public T
		{
			Array( GlslWriter * p_writer, const Castor::String & p_name, uint32_t p_dimension );
			template< typename U > T & operator[]( U const & p_offset ) { *m_writer << m_name << cuT( "[" ) << p_offset << cuT( "]" );return *this; }
			uint32_t m_dimension;
		};

		//! Language keywords
		struct Version{};
		struct Attribute{};
		struct In{};
		struct Out{};
		struct Layout{};
		struct Uniform{};
		struct StdLayout
		{
			int m_index;
		};

		//! No UBO
		struct Legacy_MatrixOut{};
		struct Legacy_MatrixCopy{};

		//! Miscellaneous
		struct Endl{};
		
		struct IndentBlock
		{
			IndentBlock( GlslWriter & p_writter );
			~IndentBlock();
			Castor::StringStream & m_stream;
			int m_indent;
		};

		struct Ubo
		{
			Ubo( GlslWriter & p_writer, Castor::String const & p_name );
			void End();
			template< typename T > inline T GetUniform( Castor::String const & p_name );
			template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
			IndentBlock * m_block;
			GlslWriter & m_writer;
			Castor::String m_name;
		};

		struct Struct
		{
			Struct( GlslWriter & p_writer, Castor::String const & p_name );
			void End();
			template< typename T > inline T GetMember( Castor::String const & p_name );
			template< typename T > inline Array< T > GetMember( Castor::String const & p_name, uint32_t p_dimension );
			IndentBlock * m_block;
			GlslWriter & m_writer;
			Castor::String m_name;
		};

		class GlslWriter
		{
			friend struct IndentBlock;
			friend struct Ubo;

		public:
			GlslWriter( OpenGl & p_gl, Castor3D::eSHADER_TYPE p_type );

			Castor::String Finalise();
			void WriteAssign( Type const & p_lhs, Type const & p_rhs );
			void WriteAssign( Type const & p_lhs, int const & p_rhs );
			void WriteAssign( Type const & p_lhs, float const & p_rhs );
			void For( Type const & p_init, Expr const & p_cond, Expr const & p_incr, std::function< void( Type ) > p_function );
			GlslWriter & If( Expr const & p_cond, std::function< void() > p_function );
			GlslWriter & ElseIf( Expr const & p_cond, std::function< void() > p_function );
			void Else( std::function< void() > p_function );
			Struct GetStruct( Castor::String const & p_name );
			Ubo GetUbo( Castor::String const & p_name );
			void EmitVertex();
			void EndPrimitive();

			template< typename RetType, typename FuncType, typename ... Params > inline void Implement_Function( Castor::String const & p_name, FuncType p_function, Params && ... p_params );
			template< typename RetType > void Return( RetType const & p_return );
			template< typename ExprType > ExprType Paren( ExprType const p_expr );
			template< typename T > inline T Cast( Type const & p_from );
			template< typename T > inline T GetAttribute( Castor::String const & p_name );
			template< typename T > inline T GetOut( Castor::String const & p_name );
			template< typename T > inline T GetIn( Castor::String const & p_name );
			template< typename T > inline T GetLocale( Castor::String const & p_name );
			template< typename T > inline T GetLocale( Castor::String const & p_name, Expr const & p_rhs );
			template< typename T > inline T GetLocale( Castor::String const & p_name, Type const & p_rhs );
			template< typename T > inline T GetBuiltin( Castor::String const & p_name );
			template< typename T > inline T GetLayout( Castor::String const & p_name );
			template< typename T > inline T GetUniform( Castor::String const & p_name );
			template< typename T > inline Array< T > GetAttribute( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetOut( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetIn( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Expr const & p_rhs );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Type const & p_rhs );
			template< typename T > inline Array< T > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetLayout( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
		
			GlslWriter & operator<<( Version const & p_rhs );
			GlslWriter & operator<<( Attribute const & p_rhs );
			GlslWriter & operator<<( In const & p_rhs );
			GlslWriter & operator<<( Out const & p_rhs );
			GlslWriter & operator<<( StdLayout const & p_rhs );
			GlslWriter & operator<<( Layout const & p_rhs );
			GlslWriter & operator<<( Uniform const & p_rhs );

			GlslWriter & operator<<( Legacy_MatrixOut const & p_rhs );
			GlslWriter & operator<<( Legacy_MatrixCopy const & p_rhs );

			GlslWriter & operator<<( Endl const & p_rhs );
			GlslWriter & operator<<( Castor::String const & p_rhs );
			GlslWriter & operator<<( uint32_t const & p_rhs );

		private:
			Castor::String m_uniform;
			VariablesBase * m_variables;
			ConstantsBase * m_constants;
			std::unique_ptr< KeywordsBase > m_keywords;
			Castor::StringStream m_stream;
			int m_attributeIndex;
			int m_layoutIndex;
			Castor3D::eSHADER_TYPE m_type;
			OpenGl & m_gl;
		};

		//! Types
		struct Sampler1D: public Type
		{
			Sampler1D(): Type( cuT( "sampler1D " ) ){}
			Sampler1D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "sampler1D " ), p_writer, p_name ){}
			template< typename T > inline Sampler1D & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Sampler2D: public Type
		{
			Sampler2D(): Type( cuT( "sampler2D " ) ){}
			Sampler2D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "sampler2D " ), p_writer, p_name ){}
			template< typename T > inline Sampler2D & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Sampler3D: public Type
		{
			Sampler3D(): Type( cuT( "sampler3D " ) ){}
			Sampler3D( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "sampler3D " ), p_writer, p_name ){}
			template< typename T > inline Sampler3D & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Void: public Type
		{
			Void(): Type( cuT( "void " ) ){}
			Void( GlslWriter * p_writer ): Type( cuT( "void " ), p_writer, Castor::String() ){}
		};

		struct Int: public Type
		{
			Int(): Type( cuT( "int " ) ){}
			Int( Type const & p_value ): Type( cuT( "float " ) ) { m_value << Castor::String( p_value ); }
			Int( int p_value ): Type( cuT( "int " ) ) { m_value << p_value; }
			Int( float p_value ): Type( cuT( "int " ) ) { m_value << p_value; }
			Int( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "int " ), p_writer, p_name ){}
			inline Int & operator=( Int const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Int & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Int & operator=( int p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			inline operator uint32_t() { return 0u; }
			inline Int & operator++() { m_value << cuT( "++" ) << Castor::String( *this );return *this; }
			inline Int operator++( int ) { Int l_return;l_return.m_value << Castor::String( *this ) << cuT( "++" );return l_return; }
		};

		struct Float: public Type
		{
			Float(): Type( cuT( "float " ) ){}
			Float( Type const & p_value ): Type( cuT( "float " ) ) { m_value << Castor::String( p_value ); }
			Float( int p_value ): Type( cuT( "float " ) ) { m_value << p_value; }
			Float( float p_value ): Type( cuT( "float " ) ) { m_value << p_value; }
			Float( GlslWriter * p_writer, int p_value ): Type( cuT( "float " ), p_writer, Castor::String() ) { m_value << p_value; }
			Float( GlslWriter * p_writer, float p_value ): Type( cuT( "float " ), p_writer, Castor::String() ) { m_value << p_value; }
			Float( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "float " ), p_writer, p_name ){}
			Float & operator=( Float const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Float & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Float & operator=( float p_rhs ){ *m_writer << Castor::String( *this ) << cuT( " = " ) << Castor::str_utils::to_string( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator float() { return 0.0f; }
		};
		struct Vec2: public Type
		{
			Vec2(): Type( cuT( "vec2 " ) ){}
			Vec2( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "vec2 " ), p_writer, p_name ){}
			inline Vec2 & operator=( Vec2 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec2 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec2 & operator[]( T const & p_offset ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Float x() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Float y() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Float r() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Float g() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
		};

		struct Vec3: public Type
		{
			Vec3(): Type( cuT( "vec3 " ) ){}
			Vec3( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "vec3 " ), p_writer, p_name ){}
			inline Vec3 & operator=( Vec3 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec3 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec3 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Float x() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Float y() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Float z() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".z" );return l_return; }
			inline Float r() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Float g() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
			inline Float b() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".b" );return l_return; }
			inline Vec2 xy() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xy" );return l_return; }
			inline Vec2 xz() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xz" );return l_return; }
			inline Vec2 yx() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yx" );return l_return; }
			inline Vec2 yz() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yz" );return l_return; }
			inline Vec2 zx() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zx" );return l_return; }
			inline Vec2 zy() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zy" );return l_return; }
			inline Vec2 zw() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zw" );return l_return; }
			inline Vec2 rg() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rg" );return l_return; }
			inline Vec2 rb() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rb" );return l_return; }
			inline Vec2 gr() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gr" );return l_return; }
			inline Vec2 gb() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gb" );return l_return; }
			inline Vec2 br() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".br" );return l_return; }
			inline Vec2 bg() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bg" );return l_return; }
			inline Vec3 xyz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyz" );return l_return; }
			inline Vec3 xzy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzy" );return l_return; }
			inline Vec3 yxz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxz" );return l_return; }
			inline Vec3 yzx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzx" );return l_return; }
			inline Vec3 zxy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxy" );return l_return; }
			inline Vec3 zyx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyx" );return l_return; }
			inline Vec3 rgb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgb" );return l_return; }
			inline Vec3 rbg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbg" );return l_return; }
			inline Vec3 grb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grb" );return l_return; }
			inline Vec3 gbr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbr" );return l_return; }
			inline Vec3 brg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brg" );return l_return; }
			inline Vec3 bgr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgr" );return l_return; }
		};

		struct Vec4: public Type
		{
			Vec4(): Type( cuT( "vec4 " ) ){}
			Vec4( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "vec4 " ), p_writer, p_name ){}
			inline Vec4 & operator=( Vec4 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec4 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Vec4 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Float x() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Float y() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Float z() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".z" );return l_return; }
			inline Float w() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".w" );return l_return; }
			inline Float r() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Float g() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
			inline Float b() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".b" );return l_return; }
			inline Float a() { Float l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".a" );return l_return; }
			inline Vec2 xy() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xy" );return l_return; }
			inline Vec2 xz() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xz" );return l_return; }
			inline Vec2 xw() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xw" );return l_return; }
			inline Vec2 yx() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yx" );return l_return; }
			inline Vec2 yz() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yz" );return l_return; }
			inline Vec2 yw() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yw" );return l_return; }
			inline Vec2 zx() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zx" );return l_return; }
			inline Vec2 zy() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zy" );return l_return; }
			inline Vec2 zw() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zw" );return l_return; }
			inline Vec2 wx() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wx" );return l_return; }
			inline Vec2 wy() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wy" );return l_return; }
			inline Vec2 wz() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wz" );return l_return; }
			inline Vec2 rg() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rg" );return l_return; }
			inline Vec2 rb() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rb" );return l_return; }
			inline Vec2 ra() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ra" );return l_return; }
			inline Vec2 gr() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gr" );return l_return; }
			inline Vec2 gb() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gb" );return l_return; }
			inline Vec2 ga() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ga" );return l_return; }
			inline Vec2 br() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".br" );return l_return; }
			inline Vec2 bg() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bg" );return l_return; }
			inline Vec2 ba() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ba" );return l_return; }
			inline Vec2 ar() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ar" );return l_return; }
			inline Vec2 ag() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ag" );return l_return; }
			inline Vec2 ab() { Vec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ab" );return l_return; }
			inline Vec3 xyz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyz" );return l_return; }
			inline Vec3 xyw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyw" );return l_return; }
			inline Vec3 xzy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzy" );return l_return; }
			inline Vec3 xzw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzw" );return l_return; }
			inline Vec3 xwy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwy" );return l_return; }
			inline Vec3 xwz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwz" );return l_return; }
			inline Vec3 yxz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxz" );return l_return; }
			inline Vec3 yxw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxw" );return l_return; }
			inline Vec3 yzx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzx" );return l_return; }
			inline Vec3 yzw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzw" );return l_return; }
			inline Vec3 ywx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywx" );return l_return; }
			inline Vec3 ywz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywz" );return l_return; }
			inline Vec3 zxy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxy" );return l_return; }
			inline Vec3 zxw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxw" );return l_return; }
			inline Vec3 zyx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyx" );return l_return; }
			inline Vec3 zyw() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyw" );return l_return; }
			inline Vec3 zwx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwx" );return l_return; }
			inline Vec3 zwy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwy" );return l_return; }
			inline Vec3 wxy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxy" );return l_return; }
			inline Vec3 wxz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxz" );return l_return; }
			inline Vec3 wyx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyx" );return l_return; }
			inline Vec3 wyz() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyz" );return l_return; }
			inline Vec3 wzx() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzx" );return l_return; }
			inline Vec3 wzy() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzy" );return l_return; }
			inline Vec3 rgb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgb" );return l_return; }
			inline Vec3 rga() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rga" );return l_return; }
			inline Vec3 rbg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbg" );return l_return; }
			inline Vec3 rba() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rba" );return l_return; }
			inline Vec3 rag() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rag" );return l_return; }
			inline Vec3 rab() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rab" );return l_return; }
			inline Vec3 grb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grb" );return l_return; }
			inline Vec3 gra() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gra" );return l_return; }
			inline Vec3 gbr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbr" );return l_return; }
			inline Vec3 gba() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gba" );return l_return; }
			inline Vec3 gar() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gar" );return l_return; }
			inline Vec3 gab() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gab" );return l_return; }
			inline Vec3 brg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brg" );return l_return; }
			inline Vec3 bra() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bra" );return l_return; }
			inline Vec3 bgr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgr" );return l_return; }
			inline Vec3 bga() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bga" );return l_return; }
			inline Vec3 bar() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bar" );return l_return; }
			inline Vec3 bag() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bag" );return l_return; }
			inline Vec3 arg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arg" );return l_return; }
			inline Vec3 arb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arb" );return l_return; }
			inline Vec3 agr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agr" );return l_return; }
			inline Vec3 agb() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agb" );return l_return; }
			inline Vec3 abr() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abr" );return l_return; }
			inline Vec3 abg() { Vec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abg" );return l_return; }
			inline Vec4 xyzw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyzw" );return l_return; }
			inline Vec4 xywz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xywz" );return l_return; }
			inline Vec4 xzyw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzyw" );return l_return; }
			inline Vec4 xzwy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzwy" );return l_return; }
			inline Vec4 xwyz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwyz" );return l_return; }
			inline Vec4 xwzy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwzy" );return l_return; }
			inline Vec4 yxzw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxzw" );return l_return; }
			inline Vec4 yxwz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxwz" );return l_return; }
			inline Vec4 yzxw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzxw" );return l_return; }
			inline Vec4 yzwx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzwx" );return l_return; }
			inline Vec4 ywxz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywxz" );return l_return; }
			inline Vec4 ywzx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywzx" );return l_return; }
			inline Vec4 zxyw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxyw" );return l_return; }
			inline Vec4 zxwy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxwy" );return l_return; }
			inline Vec4 zyxw() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyxw" );return l_return; }
			inline Vec4 zywx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zywx" );return l_return; }
			inline Vec4 zwxy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwxy" );return l_return; }
			inline Vec4 zwyx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwyx" );return l_return; }
			inline Vec4 wxyz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxyz" );return l_return; }
			inline Vec4 wxzy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxzy" );return l_return; }
			inline Vec4 wyxz() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyxz" );return l_return; }
			inline Vec4 wyzx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyzx" );return l_return; }
			inline Vec4 wzxy() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzxy" );return l_return; }
			inline Vec4 wzyx() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzyx" );return l_return; }
			inline Vec4 rgba() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgba" );return l_return; }
			inline Vec4 rgab() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgab" );return l_return; }
			inline Vec4 rbga() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbga" );return l_return; }
			inline Vec4 rbag() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbag" );return l_return; }
			inline Vec4 ragb() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ragb" );return l_return; }
			inline Vec4 rabg() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rabg" );return l_return; }
			inline Vec4 grba() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grba" );return l_return; }
			inline Vec4 grab() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grab" );return l_return; }
			inline Vec4 gbra() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbra" );return l_return; }
			inline Vec4 gbar() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbar" );return l_return; }
			inline Vec4 garb() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".garb" );return l_return; }
			inline Vec4 gabr() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gabr" );return l_return; }
			inline Vec4 brga() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brga" );return l_return; }
			inline Vec4 brag() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brag" );return l_return; }
			inline Vec4 bgra() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgra" );return l_return; }
			inline Vec4 bgar() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgar" );return l_return; }
			inline Vec4 barg() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".barg" );return l_return; }
			inline Vec4 bagr() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bagr" );return l_return; }
			inline Vec4 argb() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".argb" );return l_return; }
			inline Vec4 arbg() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arbg" );return l_return; }
			inline Vec4 agrb() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agrb" );return l_return; }
			inline Vec4 agbr() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agbr" );return l_return; }
			inline Vec4 abrg() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abrg" );return l_return; }
			inline Vec4 abgr() { Vec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abgr" );return l_return; }
		};

		struct IVec2: public Type
		{
			IVec2(): Type( cuT( "ivec2 " ) ){}
			IVec2( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "ivec2 " ), p_writer, p_name ){}
			inline IVec2 & operator=( IVec2 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec2 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec2 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Int x() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Int y() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Int r() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Int g() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
		};

		struct IVec3: public Type
		{
			IVec3(): Type( cuT( "ivec3 " ) ){}
			IVec3( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "ivec3 " ), p_writer, p_name ){}
			inline IVec3 & operator=( IVec3 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec3 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec3 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Int x() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Int y() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Int z() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".z" );return l_return; }
			inline Int r() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Int g() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
			inline Int b() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".b" );return l_return; }
			inline IVec2 xy() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xy" );return l_return; }
			inline IVec2 xz() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xz" );return l_return; }
			inline IVec2 yx() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yx" );return l_return; }
			inline IVec2 yz() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yz" );return l_return; }
			inline IVec2 zx() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zx" );return l_return; }
			inline IVec2 zy() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zy" );return l_return; }
			inline IVec2 zw() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zw" );return l_return; }
			inline IVec2 rg() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rg" );return l_return; }
			inline IVec2 rb() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rb" );return l_return; }
			inline IVec2 gr() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gr" );return l_return; }
			inline IVec2 gb() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gb" );return l_return; }
			inline IVec2 br() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".br" );return l_return; }
			inline IVec2 bg() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bg" );return l_return; }
			inline IVec3 xyz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyz" );return l_return; }
			inline IVec3 xzy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzy" );return l_return; }
			inline IVec3 yxz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxz" );return l_return; }
			inline IVec3 yzx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzx" );return l_return; }
			inline IVec3 zxy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxy" );return l_return; }
			inline IVec3 zyx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyx" );return l_return; }
			inline IVec3 rgb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgb" );return l_return; }
			inline IVec3 rbg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbg" );return l_return; }
			inline IVec3 grb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grb" );return l_return; }
			inline IVec3 gbr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbr" );return l_return; }
			inline IVec3 brg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brg" );return l_return; }
			inline IVec3 bgr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgr" );return l_return; }
		};

		struct IVec4: public Type
		{
			IVec4(): Type( cuT( "ivec4 " ) ){}
			IVec4( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "ivec4 " ), p_writer, p_name ){}
			inline IVec4 & operator=( IVec4 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec4 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline IVec4 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Int x() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".x" );return l_return; }
			inline Int y() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".y" );return l_return; }
			inline Int z() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".z" );return l_return; }
			inline Int w() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".w" );return l_return; }
			inline Int r() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".r" );return l_return; }
			inline Int g() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".g" );return l_return; }
			inline Int b() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".b" );return l_return; }
			inline Int a() { Int l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".a" );return l_return; }
			inline IVec2 xy() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xy" );return l_return; }
			inline IVec2 xz() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xz" );return l_return; }
			inline IVec2 xw() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xw" );return l_return; }
			inline IVec2 yx() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yx" );return l_return; }
			inline IVec2 yz() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yz" );return l_return; }
			inline IVec2 yw() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yw" );return l_return; }
			inline IVec2 zx() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zx" );return l_return; }
			inline IVec2 zy() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zy" );return l_return; }
			inline IVec2 zw() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zw" );return l_return; }
			inline IVec2 wx() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wx" );return l_return; }
			inline IVec2 wy() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wy" );return l_return; }
			inline IVec2 wz() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wz" );return l_return; }
			inline IVec2 rg() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rg" );return l_return; }
			inline IVec2 rb() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rb" );return l_return; }
			inline IVec2 ra() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ra" );return l_return; }
			inline IVec2 gr() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gr" );return l_return; }
			inline IVec2 gb() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gb" );return l_return; }
			inline IVec2 ga() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ga" );return l_return; }
			inline IVec2 br() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".br" );return l_return; }
			inline IVec2 bg() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bg" );return l_return; }
			inline IVec2 ba() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ba" );return l_return; }
			inline IVec2 ar() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ar" );return l_return; }
			inline IVec2 ag() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ag" );return l_return; }
			inline IVec2 ab() { IVec2 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ab" );return l_return; }
			inline IVec3 xyz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyz" );return l_return; }
			inline IVec3 xyw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyw" );return l_return; }
			inline IVec3 xzy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzy" );return l_return; }
			inline IVec3 xzw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzw" );return l_return; }
			inline IVec3 xwy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwy" );return l_return; }
			inline IVec3 xwz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwz" );return l_return; }
			inline IVec3 yxz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxz" );return l_return; }
			inline IVec3 yxw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxw" );return l_return; }
			inline IVec3 yzx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzx" );return l_return; }
			inline IVec3 yzw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzw" );return l_return; }
			inline IVec3 ywx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywx" );return l_return; }
			inline IVec3 ywz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywz" );return l_return; }
			inline IVec3 zxy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxy" );return l_return; }
			inline IVec3 zxw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxw" );return l_return; }
			inline IVec3 zyx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyx" );return l_return; }
			inline IVec3 zyw() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyw" );return l_return; }
			inline IVec3 zwx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwx" );return l_return; }
			inline IVec3 zwy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwy" );return l_return; }
			inline IVec3 wxy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxy" );return l_return; }
			inline IVec3 wxz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxz" );return l_return; }
			inline IVec3 wyx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyx" );return l_return; }
			inline IVec3 wyz() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyz" );return l_return; }
			inline IVec3 wzx() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzx" );return l_return; }
			inline IVec3 wzy() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzy" );return l_return; }
			inline IVec3 rgb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgb" );return l_return; }
			inline IVec3 rga() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rga" );return l_return; }
			inline IVec3 rbg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbg" );return l_return; }
			inline IVec3 rba() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rba" );return l_return; }
			inline IVec3 rag() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rag" );return l_return; }
			inline IVec3 rab() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rab" );return l_return; }
			inline IVec3 grb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grb" );return l_return; }
			inline IVec3 gra() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gra" );return l_return; }
			inline IVec3 gbr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbr" );return l_return; }
			inline IVec3 gba() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gba" );return l_return; }
			inline IVec3 gar() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gar" );return l_return; }
			inline IVec3 gab() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gab" );return l_return; }
			inline IVec3 brg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brg" );return l_return; }
			inline IVec3 bra() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bra" );return l_return; }
			inline IVec3 bgr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgr" );return l_return; }
			inline IVec3 bga() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bga" );return l_return; }
			inline IVec3 bar() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bar" );return l_return; }
			inline IVec3 bag() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bag" );return l_return; }
			inline IVec3 arg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arg" );return l_return; }
			inline IVec3 arb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arb" );return l_return; }
			inline IVec3 agr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agr" );return l_return; }
			inline IVec3 agb() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agb" );return l_return; }
			inline IVec3 abr() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abr" );return l_return; }
			inline IVec3 abg() { IVec3 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abg" );return l_return; }
			inline IVec4 xyzw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xyzw" );return l_return; }
			inline IVec4 xywz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xywz" );return l_return; }
			inline IVec4 xzyw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzyw" );return l_return; }
			inline IVec4 xzwy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xzwy" );return l_return; }
			inline IVec4 xwyz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwyz" );return l_return; }
			inline IVec4 xwzy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".xwzy" );return l_return; }
			inline IVec4 yxzw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxzw" );return l_return; }
			inline IVec4 yxwz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yxwz" );return l_return; }
			inline IVec4 yzxw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzxw" );return l_return; }
			inline IVec4 yzwx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".yzwx" );return l_return; }
			inline IVec4 ywxz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywxz" );return l_return; }
			inline IVec4 ywzx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ywzx" );return l_return; }
			inline IVec4 zxyw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxyw" );return l_return; }
			inline IVec4 zxwy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zxwy" );return l_return; }
			inline IVec4 zyxw() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zyxw" );return l_return; }
			inline IVec4 zywx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zywx" );return l_return; }
			inline IVec4 zwxy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwxy" );return l_return; }
			inline IVec4 zwyx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".zwyx" );return l_return; }
			inline IVec4 wxyz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxyz" );return l_return; }
			inline IVec4 wxzy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wxzy" );return l_return; }
			inline IVec4 wyxz() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyxz" );return l_return; }
			inline IVec4 wyzx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wyzx" );return l_return; }
			inline IVec4 wzxy() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzxy" );return l_return; }
			inline IVec4 wzyx() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".wzyx" );return l_return; }
			inline IVec4 rgba() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgba" );return l_return; }
			inline IVec4 rgab() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rgab" );return l_return; }
			inline IVec4 rbga() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbga" );return l_return; }
			inline IVec4 rbag() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rbag" );return l_return; }
			inline IVec4 ragb() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".ragb" );return l_return; }
			inline IVec4 rabg() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".rabg" );return l_return; }
			inline IVec4 grba() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grba" );return l_return; }
			inline IVec4 grab() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".grab" );return l_return; }
			inline IVec4 gbra() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbra" );return l_return; }
			inline IVec4 gbar() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gbar" );return l_return; }
			inline IVec4 garb() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".garb" );return l_return; }
			inline IVec4 gabr() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".gabr" );return l_return; }
			inline IVec4 brga() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brga" );return l_return; }
			inline IVec4 brag() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".brag" );return l_return; }
			inline IVec4 bgra() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgra" );return l_return; }
			inline IVec4 bgar() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bgar" );return l_return; }
			inline IVec4 barg() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".barg" );return l_return; }
			inline IVec4 bagr() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".bagr" );return l_return; }
			inline IVec4 argb() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".argb" );return l_return; }
			inline IVec4 arbg() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".arbg" );return l_return; }
			inline IVec4 agrb() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agrb" );return l_return; }
			inline IVec4 agbr() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".agbr" );return l_return; }
			inline IVec4 abrg() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abrg" );return l_return; }
			inline IVec4 abgr() { IVec4 l_return( m_writer );l_return.m_value << Castor::String( *this ) << cuT( ".abgr" );return l_return; }
		};

		struct Mat3: public Type
		{
			Mat3(): Type( cuT( "mat3 " ) ){}
			Mat3( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "mat3 " ), p_writer, p_name ){}
			inline Mat3 & operator=( Mat3 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Mat3 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Mat3 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct Mat4: public Type
		{
			Mat4(): Type( cuT( "mat4 " ) ){}
			Mat4( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "mat4 " ), p_writer, p_name ){}
			inline Mat4 & operator=( Mat4 const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Mat4 & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Mat4 & operator[]( T const & p_rhs ) { m_value << Castor::String( *this ) << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct Light: public Type
		{
			Light(): Type( cuT( "Light " ) ){}
			Light( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "Light " ), p_writer, p_name ){}
			inline Light & operator=( Light const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline Light & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			Vec4 m_v4Ambient()const { return Vec4( m_writer, m_name + cuT( ".m_v4Ambient" ) ); }
			Vec4 m_v4Diffuse()const { return Vec4( m_writer, m_name + cuT( ".m_v4Diffuse" ) ); }
			Vec4 m_v4Specular()const { return Vec4( m_writer, m_name + cuT( ".m_v4Specular" ) ); }
			Vec4 m_v4Position()const { return Vec4( m_writer, m_name + cuT( ".m_v4Position" ) ); }
			Int m_iType()const { return Int( m_writer, m_name + cuT( ".m_iType" ) ); }
			Vec3 m_v3Attenuation()const { return Vec3( m_writer, m_name + cuT( ".m_v3Attenuation" ) ); }
			Mat4 m_mtx4Orientation()const { return Mat4( m_writer, m_name + cuT( ".m_mtx4Orientation" ) ); }
			Float m_fExponent()const { return Float( m_writer, m_name + cuT( ".m_fExponent" ) ); }
			Float m_fCutOff()const { return Float( m_writer, m_name + cuT( ".m_fCutOff" ) ); }
		};

		struct gl_PerVertex: public Type
		{
			gl_PerVertex(): Type( cuT( "gl_PerVertex " ) ){}
			gl_PerVertex( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() ): Type( cuT( "gl_PerVertex " ), p_writer, p_name ){}
			inline gl_PerVertex & operator=( gl_PerVertex const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			template< typename T > inline gl_PerVertex & operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
			Vec4 gl_Position()const { return Vec4( m_writer, m_name + cuT( ".gl_Position" ) ); }
			Float gl_PointSize()const { return Float( m_writer, m_name + cuT( ".gl_PointSize" ) ); }
			Float gl_ClipDistance()const { return Array< Float >( m_writer, m_name + cuT( ".gl_ClipDistance" ), 8 ); }
		};

		template< typename TypeT >
		struct InParam: public TypeT
		{
			InParam( GlslWriter * p_writer, Castor::String const & p_name ): TypeT( p_writer, p_name ){ TypeT::m_type = cuT( "in " ) + TypeT::m_type; }
		};

		template< typename TypeT >
		struct OutParam: public TypeT
		{
			OutParam( GlslWriter * p_writer, Castor::String const & p_name ): TypeT( p_writer, p_name ){ TypeT::m_type = cuT( "out " ) + TypeT::m_type; }
			template< typename T > inline OutParam< TypeT > operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
		};

		template< typename TypeT >
		struct InOutParam: public TypeT
		{
			InOutParam( GlslWriter * p_writer, Castor::String const & p_name ): TypeT( p_writer, p_name ){ TypeT::m_type = cuT( "inout " ) + TypeT::m_type; }
			template< typename T > inline InOutParam< TypeT > operator=( T const & p_rhs ){ m_writer->WriteAssign( *this, p_rhs );return *this; }
		};
		
		template< typename T > struct is_arithmetic_type : public std::false_type {};
		template<> struct is_arithmetic_type< Int > : public std::true_type {};
		template<> struct is_arithmetic_type< Float > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec2 > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec3 > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec4 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec2 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec3 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec4 > : public std::true_type {};
		template<> struct is_arithmetic_type< Mat3 > : public std::true_type {};
		template<> struct is_arithmetic_type< Mat4 > : public std::true_type {};

		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator+( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator-( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator*( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator/( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator==( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator!=( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator<( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator<=( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator>( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable=typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >Bool operator>=( TypeA const & p_a, TypeB const & p_b );
		
		Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b );

		template< typename ... Values > inline Vec2 vec2( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Vec3 vec3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Vec4 vec4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec2 ivec2( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec3 ivec3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec4 ivec4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Mat3 mat3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Mat4 mat4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float dot( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float inversesqrt( Expr const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float sqrt( Expr const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float pow( Expr const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value cross( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value clamp( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value min( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value max( Value const & p_value, Values const & ... p_values );
		template< typename Value > inline Value neg( Value const & p_value );
		template< typename Value > inline Value normalize( Value const & p_value );
		template< typename Value > inline Value transpose( Value const & p_value );
		template< typename Value > inline Value inverse( Value const & p_value );
		Vec4 texture1D( Sampler1D const & p_sampler, Type const & p_value );
		Vec4 texture2D( Sampler2D const & p_sampler, Type const & p_value );
		Vec4 texture3D( Sampler3D const & p_sampler, Type const & p_value );
		Vec2 reflect( Vec2 const & p_a, Type const & p_b );
		Vec3 reflect( Vec3 const & p_a, Type const & p_b );
		Vec4 reflect( Vec4 const & p_a, Type const & p_b );
		Float length( Type const & p_value );
		Float radians( Type const & p_value );
		Float cos( Type const & p_value );
		Float sin( Type const & p_value );
		Float tan( Type const & p_value );

		template< typename LightingModel >
		class Lighting
		{
		public:
			// Common ones
			void Declare_Light( GlslWriter & p_writer );
			void Declare_GetLight( GlslWriter & p_writer );
			// LightingModel specifics
			void Declare_ComputeLightDirection( GlslWriter & p_writer );
			void Declare_Bump( GlslWriter & p_writer );
			void Declare_ComputeFresnel( GlslWriter & p_writer );
			// Calls
			inline Light GetLight( Type const & p_value );
			template< typename ... Values > inline Vec4 ComputeLightDirection( Type const & p_value, Values const & ... p_values );
			template< typename ... Values > inline void Bump( Type const & p_value, Values const & ... p_values );
			template< typename ... Values > inline Float ComputeFresnel( Type const & p_value, Values const & ... p_values );
		};

		struct BlinnPhongLightingModel
		{
			static void Declare_ComputeLightDirection( GlslWriter & p_writer );
			static void Declare_Bump( GlslWriter & p_writer );
			static void Declare_ComputeFresnel( GlslWriter & p_writer );
		};
	}

#define FOR( Writer, Type, Name, Init, Cond, Incr )\
	Type Name( &Writer, cuT( #Name ) );\
	Name.m_value << Type().m_type << cuT( #Name ) << cuT( " = " ) << cuT( #Init );\
	( Writer ).For( Name, Expr( &( Writer ), Castor::String( Cond ) ), Expr( &( Writer ), Castor::String( Incr ) ), [&]( Type i )

#define ROF\
	);

#define IF( Writer, Condition )\
	( Writer ).If( Expr( &( Writer ), Castor::String( Condition ) ), [&]()

#define ELSE\
	).Else( [&]()

#define ELSEIF()\
	).ElseIf( [&]()

#define FI\
	);

#define LOCALE( Writer, Type, Name )\
	Type Name = ( Writer ).GetLocale< Type >( cuT( #Name ) )

#define LOCALE_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension )

#define LOCALE_ASSIGN( Writer, Type, Name, Assign )\
	Type Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Assign )

#define LOCALE_ASSIGN_ARRAY( Writer, Type, Name, Dimension, Assign )\
	Array< Type > Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension, Assign )

#define BUILTIN( Writer, Type, Name )\
	Type Name = ( Writer ).GetBuiltin< Type >( cuT( #Name ) )

#define BUILTIN_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetBuiltin< Type >( cuT( #Name ), Dimension )

#define OUT( Writer, Type, Name )\
	Type Name = ( Writer ).GetOut< Type >( cuT( #Name ) )

#define OUT_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetOut< Type >( cuT( #Name ), Dimension )

#define IN( Writer, Type, Name )\
	Type Name = ( Writer ).GetIn< Type >( cuT( #Name ) )

#define IN_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetIn< Type >( cuT( #Name ), Dimension )

#define ATTRIBUTE( Writer, Type, Name )\
	Type Name = ( Writer ).GetAttribute< Type >( cuT( #Name ) )

#define ATTRIBUTE_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetAttribute< Type >( cuT( #Name ), Dimension )

#define UNIFORM( Writer, Type, Name )\
	Type Name = ( Writer ).GetUniform< Type >( cuT( #Name ) )

#define UNIFORM_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetUniform< Type >( cuT( #Name ), Dimension )

#define LAYOUT( Writer, Type, Name )\
	Type Name = ( Writer ).GetLayout< Type >( cuT( #Name ) )

#define LAYOUT_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetLayout< Type >( cuT( #Name ), Dimension )

#define CAST( Writer, NewType, OldType )\
	( Writer ).Cast< NewType >( OldType )

#define UBO_MATRIX( Writer )\
	Ubo l_matrices = l_writer.GetUbo( cuT( "Matrices" ) );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjection );\
	UNIFORM( l_matrices, Mat4, c3d_mtxModel );\
	UNIFORM( l_matrices, Mat4, c3d_mtxView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxModelView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjectionView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjectionModelView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxNormal );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture0 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture1 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture2 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture3 );\
	UNIFORM_ARRAY( l_matrices, Mat4, c3d_mtxBones, 100 );\
	l_matrices.End();

#define UBO_PASS( Writer )\
	Ubo l_pass = l_writer.GetUbo( cuT( "Pass" ) );\
	UNIFORM( l_pass, Vec4, c3d_v4MatAmbient );\
	UNIFORM( l_pass, Vec4, c3d_v4MatDiffuse );\
	UNIFORM( l_pass, Vec4, c3d_v4MatEmissive );\
	UNIFORM( l_pass, Vec4, c3d_v4MatSpecular );\
	UNIFORM( l_pass, Float, c3d_fMatShininess );\
	UNIFORM( l_pass, Float, c3d_fMatOpacity );\
	l_pass.End();

#define UBO_SCENE( Writer )\
	Ubo l_scene = l_writer.GetUbo( cuT( "Scene" ) );\
	UNIFORM( l_scene, Int, c3d_iLightsCount );\
	UNIFORM( l_scene, Vec4, c3d_v4AmbientLight );\
	UNIFORM( l_scene, Vec4, c3d_v4BackgroundColour );\
	UNIFORM( l_scene, Vec3, c3d_v3CameraPosition );\
	l_scene.End();

#define UBO_BILLBOARD( Writer )\
	Ubo l_billboard = l_writer.GetUbo( cuT( "Billboard" ) );\
	UNIFORM( l_billboard, IVec2, c3d_v2iDimensions );\
	l_billboard.End();
}

#include "GlShaderSource.inl"

#endif
