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

namespace GlRender
{
	namespace GLSL
	{
		class Lighting
		{
		public:
			inline Castor::String const & GetLightsDeclaration()const
			{
				return m_strLightsDeclaration;
			}

		protected:
			Lighting();

		private:
			Castor::String m_strLightsDeclaration;
		};

		class BlinnPhong
			: public Lighting
		{
		public:
			BlinnPhong();

			inline Castor::String const & GetPixelLights()const
			{
				return m_blinnPhongFresnelShit;
			}

		private:
			Castor::String m_blinnPhongFresnelShit;
		};

		class GlslWriter;

		struct Type
		{
			Type( Castor::String const & p_type );
			Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name );
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

			GlslWriter * m_writer;
			Castor::String m_type;
			Castor::String m_name;
			mutable Castor::StringStream m_result;
		};

		Type const & operator+( Type const & p_a, Type const & p_b );
		Type const & operator-( Type const & p_a, Type const & p_b );
		Type const & operator*( Type const & p_a, Type const & p_b );
		Type const & operator/( Type const & p_a, Type const & p_b );
		Type const & operator+( Type const & p_a, float p_b );
		Type const & operator-( Type const & p_a, float p_b );
		Type const & operator*( Type const & p_a, float p_b );
		Type const & operator/( Type const & p_a, float p_b );
		Type const & operator+( Type const & p_a, int p_b );
		Type const & operator-( Type const & p_a, int p_b );
		Type const & operator*( Type const & p_a, int p_b );
		Type const & operator/( Type const & p_a, int p_b );

		template< typename T >
		struct Array: public T
		{
			Array( GlslWriter * p_writer, const Castor::String & p_name, uint32_t p_dimension )
				: T( p_writer, p_name )
				, m_dimension( p_dimension )
			{
			}

			template< typename U > T & operator[]( U const & p_offset ) { *m_writer << m_name << cuT( "[" ) << p_offset << cuT( "]" );return *this; }
			uint32_t m_dimension;
		};

		//! Language keywords
		struct Version{};
		struct Attribute{};
		struct In{};
		struct Out{};
		struct StdLayout{};
		struct Layout{};
		struct Uniform{};

		//! No UBO
		struct Legacy_MatrixOut{};
		struct Legacy_MatrixCopy{};

		//! Miscellaneous
		struct Endl{};
		struct Main{};

		//! Types
		struct Sampler1D: public Type
		{
			Sampler1D(): Type( cuT( "sampler1D " ) ){}
			Sampler1D( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "sampler1D " ), p_writer, p_name ){}
			template< typename T > inline Sampler1D & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Sampler2D: public Type
		{
			Sampler2D(): Type( cuT( "sampler2D " ) ){}
			Sampler2D( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "sampler2D " ), p_writer, p_name ){}
			template< typename T > inline Sampler2D & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Sampler3D: public Type
		{
			Sampler3D(): Type( cuT( "sampler3D " ) ){}
			Sampler3D( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "sampler3D " ), p_writer, p_name ){}
			template< typename T > inline Sampler3D & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Void: public Type
		{
			Void(): Type( cuT( "void " ) ){}
		};

		struct Int: public Type
		{
			Int(): Type( cuT( "int " ) ){}
			Int( int p_value ): Type( cuT( "int " ) ){ m_result << p_value; }
			Int( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "int " ), p_writer, p_name ){}
			template< typename T > inline Int & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Int & operator=( int p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::str_utils::to_string( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator uint32_t() { return 0u; }
		};

		struct Float: public Type
		{
			Float(): Type( cuT( "float " ) ){}
			Float( float p_value ): Type( cuT( "float " ) ){ m_result << p_value; }
			Float( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "float " ), p_writer, p_name ){}
			template< typename T > inline Float & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Float & operator=( float p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::str_utils::to_string( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			inline operator float() { return 0.0f; }
		};
		struct Vec2: public Type
		{
			Vec2(): Type( cuT( "vec2 " ) ){}
			Vec2( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "vec2 " ), p_writer, p_name ){}
			template< typename T > inline Vec2 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Vec2 & operator[]( T const & p_offset ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Vec2 & x() { m_result << m_name << cuT( ".x" );return *this; }
			inline Vec2 & y() { m_result << m_name << cuT( ".y" );return *this; }
			inline Vec2 & r() { m_result << m_name << cuT( ".r" );return *this; }
			inline Vec2 & g() { m_result << m_name << cuT( ".g" );return *this; }
		};

		struct Vec3: public Type
		{
			Vec3(): Type( cuT( "vec3 " ) ){}
			Vec3( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "vec3 " ), p_writer, p_name ){}
			template< typename T > inline Vec3 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Vec3 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Vec3 & x() { m_result << m_name << cuT( ".x" );return *this; }
			inline Vec3 & y() { m_result << m_name << cuT( ".y" );return *this; }
			inline Vec3 & z() { m_result << m_name << cuT( ".z" );return *this; }
			inline Vec3 & r() { m_result << m_name << cuT( ".r" );return *this; }
			inline Vec3 & g() { m_result << m_name << cuT( ".g" );return *this; }
			inline Vec3 & b() { m_result << m_name << cuT( ".b" );return *this; }
			inline Vec3 & xy() { m_result << m_name << cuT( ".xy" );return *this; }
			inline Vec3 & xz() { m_result << m_name << cuT( ".xz" );return *this; }
			inline Vec3 & yx() { m_result << m_name << cuT( ".yx" );return *this; }
			inline Vec3 & yz() { m_result << m_name << cuT( ".yz" );return *this; }
			inline Vec3 & zx() { m_result << m_name << cuT( ".zx" );return *this; }
			inline Vec3 & zy() { m_result << m_name << cuT( ".zy" );return *this; }
			inline Vec3 & rg() { m_result << m_name << cuT( ".rg" );return *this; }
			inline Vec3 & rb() { m_result << m_name << cuT( ".rb" );return *this; }
			inline Vec3 & gr() { m_result << m_name << cuT( ".gr" );return *this; }
			inline Vec3 & gb() { m_result << m_name << cuT( ".gb" );return *this; }
			inline Vec3 & br() { m_result << m_name << cuT( ".br" );return *this; }
			inline Vec3 & bg() { m_result << m_name << cuT( ".bg" );return *this; }
			inline Vec3 & xyz() { m_result << m_name << cuT( ".xyz" );return *this; }
			inline Vec3 & xzy() { m_result << m_name << cuT( ".xzy" );return *this; }
			inline Vec3 & yxz() { m_result << m_name << cuT( ".yxz" );return *this; }
			inline Vec3 & yzx() { m_result << m_name << cuT( ".yzx" );return *this; }
			inline Vec3 & zxy() { m_result << m_name << cuT( ".zxy" );return *this; }
			inline Vec3 & zyx() { m_result << m_name << cuT( ".zyx" );return *this; }
			inline Vec3 & rgb() { m_result << m_name << cuT( ".rgb" );return *this; }
			inline Vec3 & rbg() { m_result << m_name << cuT( ".rbg" );return *this; }
			inline Vec3 & grb() { m_result << m_name << cuT( ".grb" );return *this; }
			inline Vec3 & gbr() { m_result << m_name << cuT( ".gbr" );return *this; }
			inline Vec3 & brg() { m_result << m_name << cuT( ".brg" );return *this; }
			inline Vec3 & bgr() { m_result << m_name << cuT( ".bgr" );return *this; }
		};

		struct Vec4: public Type
		{
			Vec4(): Type( cuT( "vec4 " ) ){}
			Vec4( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "vec4 " ), p_writer, p_name ){}
			Vec4 & operator=( Vec4 const & p_rhs );
			template< typename T > inline Vec4 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Vec4 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
			inline Float x() { Float l_return;l_return.m_result << m_name << cuT( ".x" );return l_return; }
			inline Float y() { Float l_return;l_return.m_result << m_name << cuT( ".y" );return l_return; }
			inline Float z() { Float l_return;l_return.m_result << m_name << cuT( ".z" );return l_return; }
			inline Float w() { Float l_return;l_return.m_result << m_name << cuT( ".w" );return l_return; }
			inline Float r() { Float l_return;l_return.m_result << m_name << cuT( ".r" );return l_return; }
			inline Float g() { Float l_return;l_return.m_result << m_name << cuT( ".g" );return l_return; }
			inline Float b() { Float l_return;l_return.m_result << m_name << cuT( ".b" );return l_return; }
			inline Float a() { Float l_return;l_return.m_result << m_name << cuT( ".a" );return l_return; }
			inline Vec2 xy() { Vec2 l_return;l_return.m_result << m_name << cuT( ".xy" );return l_return; }
			inline Vec2 xz() { Vec2 l_return;l_return.m_result << m_name << cuT( ".xz" );return l_return; }
			inline Vec2 xw() { Vec2 l_return;l_return.m_result << m_name << cuT( ".xw" );return l_return; }
			inline Vec2 yx() { Vec2 l_return;l_return.m_result << m_name << cuT( ".yx" );return l_return; }
			inline Vec2 yz() { Vec2 l_return;l_return.m_result << m_name << cuT( ".yz" );return l_return; }
			inline Vec2 yw() { Vec2 l_return;l_return.m_result << m_name << cuT( ".yw" );return l_return; }
			inline Vec2 zx() { Vec2 l_return;l_return.m_result << m_name << cuT( ".zx" );return l_return; }
			inline Vec2 zy() { Vec2 l_return;l_return.m_result << m_name << cuT( ".zy" );return l_return; }
			inline Vec2 zw() { Vec2 l_return;l_return.m_result << m_name << cuT( ".zw" );return l_return; }
			inline Vec2 wx() { Vec2 l_return;l_return.m_result << m_name << cuT( ".wx" );return l_return; }
			inline Vec2 wy() { Vec2 l_return;l_return.m_result << m_name << cuT( ".wy" );return l_return; }
			inline Vec2 wz() { Vec2 l_return;l_return.m_result << m_name << cuT( ".wz" );return l_return; }
			inline Vec2 rg() { Vec2 l_return;l_return.m_result << m_name << cuT( ".rg" );return l_return; }
			inline Vec2 rb() { Vec2 l_return;l_return.m_result << m_name << cuT( ".rb" );return l_return; }
			inline Vec2 ra() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ra" );return l_return; }
			inline Vec2 gr() { Vec2 l_return;l_return.m_result << m_name << cuT( ".gr" );return l_return; }
			inline Vec2 gb() { Vec2 l_return;l_return.m_result << m_name << cuT( ".gb" );return l_return; }
			inline Vec2 ga() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ga" );return l_return; }
			inline Vec2 br() { Vec2 l_return;l_return.m_result << m_name << cuT( ".br" );return l_return; }
			inline Vec2 bg() { Vec2 l_return;l_return.m_result << m_name << cuT( ".bg" );return l_return; }
			inline Vec2 ba() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ba" );return l_return; }
			inline Vec2 ar() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ar" );return l_return; }
			inline Vec2 ag() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ag" );return l_return; }
			inline Vec2 ab() { Vec2 l_return;l_return.m_result << m_name << cuT( ".ab" );return l_return; }
			inline Vec3 xyz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xyz" );return l_return; }
			inline Vec3 xyw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xyw" );return l_return; }
			inline Vec3 xzy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xzy" );return l_return; }
			inline Vec3 xzw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xzw" );return l_return; }
			inline Vec3 xwy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xwy" );return l_return; }
			inline Vec3 xwz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".xwz" );return l_return; }
			inline Vec3 yxz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".yxz" );return l_return; }
			inline Vec3 yxw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".yxw" );return l_return; }
			inline Vec3 yzx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".yzx" );return l_return; }
			inline Vec3 yzw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".yzw" );return l_return; }
			inline Vec3 ywx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".ywx" );return l_return; }
			inline Vec3 ywz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".ywz" );return l_return; }
			inline Vec3 zxy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zxy" );return l_return; }
			inline Vec3 zxw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zxw" );return l_return; }
			inline Vec3 zyx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zyx" );return l_return; }
			inline Vec3 zyw() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zyw" );return l_return; }
			inline Vec3 zwx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zwx" );return l_return; }
			inline Vec3 zwy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".zwy" );return l_return; }
			inline Vec3 wxy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wxy" );return l_return; }
			inline Vec3 wxz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wxz" );return l_return; }
			inline Vec3 wyx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wyx" );return l_return; }
			inline Vec3 wyz() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wyz" );return l_return; }
			inline Vec3 wzx() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wzx" );return l_return; }
			inline Vec3 wzy() { Vec3 l_return;l_return.m_result << m_name << cuT( ".wzy" );return l_return; }
			inline Vec3 rgb() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rgb" );return l_return; }
			inline Vec3 rga() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rga" );return l_return; }
			inline Vec3 rbg() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rbg" );return l_return; }
			inline Vec3 rba() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rba" );return l_return; }
			inline Vec3 rag() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rag" );return l_return; }
			inline Vec3 rab() { Vec3 l_return;l_return.m_result << m_name << cuT( ".rab" );return l_return; }
			inline Vec3 grb() { Vec3 l_return;l_return.m_result << m_name << cuT( ".grb" );return l_return; }
			inline Vec3 gra() { Vec3 l_return;l_return.m_result << m_name << cuT( ".gra" );return l_return; }
			inline Vec3 gbr() { Vec3 l_return;l_return.m_result << m_name << cuT( ".gbr" );return l_return; }
			inline Vec3 gba() { Vec3 l_return;l_return.m_result << m_name << cuT( ".gba" );return l_return; }
			inline Vec3 gar() { Vec3 l_return;l_return.m_result << m_name << cuT( ".gar" );return l_return; }
			inline Vec3 gab() { Vec3 l_return;l_return.m_result << m_name << cuT( ".gab" );return l_return; }
			inline Vec3 brg() { Vec3 l_return;l_return.m_result << m_name << cuT( ".brg" );return l_return; }
			inline Vec3 bra() { Vec3 l_return;l_return.m_result << m_name << cuT( ".bra" );return l_return; }
			inline Vec3 bgr() { Vec3 l_return;l_return.m_result << m_name << cuT( ".bgr" );return l_return; }
			inline Vec3 bga() { Vec3 l_return;l_return.m_result << m_name << cuT( ".bga" );return l_return; }
			inline Vec3 bar() { Vec3 l_return;l_return.m_result << m_name << cuT( ".bar" );return l_return; }
			inline Vec3 bag() { Vec3 l_return;l_return.m_result << m_name << cuT( ".bag" );return l_return; }
			inline Vec3 arg() { Vec3 l_return;l_return.m_result << m_name << cuT( ".arg" );return l_return; }
			inline Vec3 arb() { Vec3 l_return;l_return.m_result << m_name << cuT( ".arb" );return l_return; }
			inline Vec3 agr() { Vec3 l_return;l_return.m_result << m_name << cuT( ".agr" );return l_return; }
			inline Vec3 agb() { Vec3 l_return;l_return.m_result << m_name << cuT( ".agb" );return l_return; }
			inline Vec3 abr() { Vec3 l_return;l_return.m_result << m_name << cuT( ".abr" );return l_return; }
			inline Vec3 abg() { Vec3 l_return;l_return.m_result << m_name << cuT( ".abg" );return l_return; }
			inline Vec4 xyzw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xyzw" );return l_return; }
			inline Vec4 xywz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xywz" );return l_return; }
			inline Vec4 xzyw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xzyw" );return l_return; }
			inline Vec4 xzwy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xzwy" );return l_return; }
			inline Vec4 xwyz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xwyz" );return l_return; }
			inline Vec4 xwzy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".xwzy" );return l_return; }
			inline Vec4 yxzw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".yxzw" );return l_return; }
			inline Vec4 yxwz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".yxwz" );return l_return; }
			inline Vec4 yzxw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".yzxw" );return l_return; }
			inline Vec4 yzwx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".yzwx" );return l_return; }
			inline Vec4 ywxz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".ywxz" );return l_return; }
			inline Vec4 ywzx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".ywzx" );return l_return; }
			inline Vec4 zxyw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zxyw" );return l_return; }
			inline Vec4 zxwy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zxwy" );return l_return; }
			inline Vec4 zyxw() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zyxw" );return l_return; }
			inline Vec4 zywx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zywx" );return l_return; }
			inline Vec4 zwxy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zwxy" );return l_return; }
			inline Vec4 zwyx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".zwyx" );return l_return; }
			inline Vec4 wxyz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wxyz" );return l_return; }
			inline Vec4 wxzy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wxzy" );return l_return; }
			inline Vec4 wyxz() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wyxz" );return l_return; }
			inline Vec4 wyzx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wyzx" );return l_return; }
			inline Vec4 wzxy() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wzxy" );return l_return; }
			inline Vec4 wzyx() { Vec4 l_return;l_return.m_result << m_name << cuT( ".wzyx" );return l_return; }
			inline Vec4 rgba() { Vec4 l_return;l_return.m_result << m_name << cuT( ".rgba" );return l_return; }
			inline Vec4 rgab() { Vec4 l_return;l_return.m_result << m_name << cuT( ".rgab" );return l_return; }
			inline Vec4 rbga() { Vec4 l_return;l_return.m_result << m_name << cuT( ".rbga" );return l_return; }
			inline Vec4 rbag() { Vec4 l_return;l_return.m_result << m_name << cuT( ".rbag" );return l_return; }
			inline Vec4 ragb() { Vec4 l_return;l_return.m_result << m_name << cuT( ".ragb" );return l_return; }
			inline Vec4 rabg() { Vec4 l_return;l_return.m_result << m_name << cuT( ".rabg" );return l_return; }
			inline Vec4 grba() { Vec4 l_return;l_return.m_result << m_name << cuT( ".grba" );return l_return; }
			inline Vec4 grab() { Vec4 l_return;l_return.m_result << m_name << cuT( ".grab" );return l_return; }
			inline Vec4 gbra() { Vec4 l_return;l_return.m_result << m_name << cuT( ".gbra" );return l_return; }
			inline Vec4 gbar() { Vec4 l_return;l_return.m_result << m_name << cuT( ".gbar" );return l_return; }
			inline Vec4 garb() { Vec4 l_return;l_return.m_result << m_name << cuT( ".garb" );return l_return; }
			inline Vec4 gabr() { Vec4 l_return;l_return.m_result << m_name << cuT( ".gabr" );return l_return; }
			inline Vec4 brga() { Vec4 l_return;l_return.m_result << m_name << cuT( ".brga" );return l_return; }
			inline Vec4 brag() { Vec4 l_return;l_return.m_result << m_name << cuT( ".brag" );return l_return; }
			inline Vec4 bgra() { Vec4 l_return;l_return.m_result << m_name << cuT( ".bgra" );return l_return; }
			inline Vec4 bgar() { Vec4 l_return;l_return.m_result << m_name << cuT( ".bgar" );return l_return; }
			inline Vec4 barg() { Vec4 l_return;l_return.m_result << m_name << cuT( ".barg" );return l_return; }
			inline Vec4 bagr() { Vec4 l_return;l_return.m_result << m_name << cuT( ".bagr" );return l_return; }
			inline Vec4 argb() { Vec4 l_return;l_return.m_result << m_name << cuT( ".argb" );return l_return; }
			inline Vec4 arbg() { Vec4 l_return;l_return.m_result << m_name << cuT( ".arbg" );return l_return; }
			inline Vec4 agrb() { Vec4 l_return;l_return.m_result << m_name << cuT( ".agrb" );return l_return; }
			inline Vec4 agbr() { Vec4 l_return;l_return.m_result << m_name << cuT( ".agbr" );return l_return; }
			inline Vec4 abrg() { Vec4 l_return;l_return.m_result << m_name << cuT( ".abrg" );return l_return; }
			inline Vec4 abgr() { Vec4 l_return;l_return.m_result << m_name << cuT( ".abgr" );return l_return; }
		};

		struct IVec2: public Type
		{
			IVec2(): Type( cuT( "ivec2 " ) ){}
			IVec2( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "ivec2 " ), p_writer, p_name ){}
			template< typename T > inline IVec2 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline IVec2 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct IVec3: public Type
		{
			IVec3(): Type( cuT( "ivec3 " ) ){}
			IVec3( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "ivec3 " ), p_writer, p_name ){}
			template< typename T > inline IVec3 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline IVec3 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct IVec4: public Type
		{
			IVec4(): Type( cuT( "ivec4 " ) ){}
			IVec4( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "ivec4 " ), p_writer, p_name ){}
			template< typename T > inline IVec4 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline IVec4 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct Mat3: public Type
		{
			Mat3(): Type( cuT( "mat3 " ) ){}
			Mat3( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "mat3 " ), p_writer, p_name ){}
			template< typename T > inline Mat3 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Mat3 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};

		struct Mat4: public Type
		{
			Mat4(): Type( cuT( "mat4 " ) ){}
			Mat4( GlslWriter * p_writer, Castor::String const & p_name ): Type( cuT( "mat4 " ), p_writer, p_name ){}
			template< typename T > inline Mat4 & operator=( T const & p_rhs ){ *m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();return *this; }
			template< typename T > inline Mat4 & operator[]( T const & p_rhs ) { m_result << m_name << cuT( "[" ) << Castor::String( p_rhs ) << cuT( "]" );return *this; }
		};
		
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

		struct Function
		{
			Function( GlslWriter & p_writer, std::function< void() > p_function );
			~Function(){}
			template< typename Return, typename ... Params >
			static Function Create( GlslWriter & p_writer, Castor::String const & p_name, std::function< void() > p_function, Params const & ... p_params );
			GlslWriter & m_writer;
		};

		class GlslWriter
		{
			friend struct IndentBlock;
			friend struct Ubo;

		public:
			GlslWriter( OpenGl & p_gl, Castor3D::eSHADER_TYPE p_type );

			Castor::String Finalise();
			Ubo GetUbo( Castor::String const & p_name );
		
			GlslWriter & operator<<( Version const & p_rhs );
			GlslWriter & operator<<( Attribute const & p_rhs );
			GlslWriter & operator<<( IVec2 const & p_rhs );
			GlslWriter & operator<<( IVec3 const & p_rhs );
			GlslWriter & operator<<( IVec4 const & p_rhs );
			GlslWriter & operator<<( Vec2 const & p_rhs );
			GlslWriter & operator<<( Vec3 const & p_rhs );
			GlslWriter & operator<<( Vec4 const & p_rhs );
			GlslWriter & operator<<( Mat4 const & p_rhs );
			GlslWriter & operator<<( Mat3 const & p_rhs );
			GlslWriter & operator<<( Sampler1D const & p_rhs );
			GlslWriter & operator<<( Sampler2D const & p_rhs );
			GlslWriter & operator<<( Sampler3D const & p_rhs );
			GlslWriter & operator<<( Void const & p_rhs );
			GlslWriter & operator<<( Int const & p_rhs );
			GlslWriter & operator<<( Float const & p_rhs );
			GlslWriter & operator<<( In const & p_rhs );
			GlslWriter & operator<<( Out const & p_rhs );
			GlslWriter & operator<<( StdLayout const & p_rhs );
			GlslWriter & operator<<( Layout const & p_rhs );
			GlslWriter & operator<<( Uniform const & p_rhs );

			GlslWriter & operator<<( Legacy_MatrixOut const & p_rhs );
			GlslWriter & operator<<( Legacy_MatrixCopy const & p_rhs );

			GlslWriter & operator<<( Endl const & p_rhs );
			GlslWriter & operator<<( Main const & p_rhs );
			GlslWriter & operator<<( Castor::String const & p_rhs );
			GlslWriter & operator<<( uint32_t const & p_rhs );

			template< typename Return, typename ... Params >
			inline Function GetFunction( Castor::String const & p_name, std::function< void() > p_function, Params const & ... p_params )
			{
				return Function::Create< Return >( *this, p_name, p_function, p_params... );
			}

			template< typename T >
			inline T GetAttribute( Castor::String const & p_name )
			{
				*this << Attribute() << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetAttribute( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << Attribute() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetOut( Castor::String const & p_name )
			{
				*this << Out() << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetOut( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << Out() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetIn( Castor::String const & p_name )
			{
				*this << Out() << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetIn( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << Out() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetLocale( Castor::String const & p_name )
			{
				*this << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << T() << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetBuiltin( Castor::String const & p_name )
			{
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension )
			{
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetLayout( Castor::String const & p_name )
			{
				*this << Layout() << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetLayout( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << Layout() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline T GetUniform( Castor::String const & p_name )
			{
				*this << Uniform() << T() << p_name << cuT( ";" ) << Endl();
				return T( this, p_name );
			}

			template< typename T >
			inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension )
			{
				*this << Uniform() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
				return Array< T >( this, p_name, p_dimension );
			}

			template< typename T >
			inline Vec4 Texture1D( Sampler1D const & p_sampler, T const & p_value )
			{
				Vec4 l_return( this );

				return l_return;
			}

			template< typename T >
			inline Vec4 texture2D( Sampler2D const & p_sampler, T const & p_value )
			{
				Vec4 l_return( this );

				return l_return;
			}

			template< typename T >
			inline Vec4 Texture3D( Sampler3D const & p_sampler, T const & p_value )
			{
				Vec4 l_return( this );

				return l_return;
			}

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

		template< typename T >
		inline T Ubo::GetUniform( Castor::String const & p_name )
		{
			m_writer << Uniform() << T() << p_name << cuT( ";" ) << Endl();
			return T( &m_writer, p_name );
		}

		template< typename T >
		inline Array< T > Ubo::GetUniform( Castor::String const & p_name, uint32_t p_dimension )
		{
			m_writer << Uniform() << T() << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
			return Array< T >( &m_writer, p_name, p_dimension );
		}
		
		namespace
		{
			template< typename Param, typename ... Params > void Construct( Castor::String & p_separator, GlslWriter & p_writer, Param const & p_current, Params const & ... p_params );

			inline void Construct( Castor::String & p_separator, GlslWriter & p_writer )
			{
				p_writer << cuT( "()" ) << Endl();
			}

			template< typename Param >
			inline void Construct( Castor::String & p_separator, GlslWriter & p_writer, Param p_last )
			{
				p_writer << p_separator << p_last << cuT( " )" ) << Endl();
			}

			template< typename Param, typename ... Params >
			inline void Construct( Castor::String & p_separator, GlslWriter & p_writer, Param const & p_current, Params const & ... p_values )
			{
				p_writer << p_separator << p_current;
				p_separator = cuT( ", " );
				Construct< Params ... > ( p_separator, p_writer, p_values... );
			}
		}
		
		template< typename Return, typename ... Params >
		inline Function Function::Create( GlslWriter & p_writer, Castor::String const & p_name, std::function< void() > p_function, Params const & ... p_params )
		{
			p_writer << Return() << p_name;
			Castor::String l_separator = cuT( "( " );
			Construct( l_separator, p_writer, p_params... );
			return Function( p_writer, p_function );
		}

		template< typename T >
		inline T normalize( T const & p_value )
		{
			T l_return( p_value.m_writer, p_value.m_name );
			l_return.m_result << cuT( "normalize( " ) << Castor::String( p_value ) << cuT( " )" );
			return l_return;
		}

		template< typename T >
		inline T transpose( T const & p_value )
		{
			T l_return( p_value.m_writer, p_value.m_name );
			l_return.m_result << cuT( "transpose( " ) << Castor::String( p_value ) << cuT( " )" );
			return l_return;
		}

		template< typename T >
		inline T inverse( T const & p_value )
		{
			T l_return( p_value.m_writer, p_value.m_name );
			l_return.m_result << cuT( "inverse( " ) << Castor::String( p_value ) << cuT( " )" );
			return l_return;
		}

		template< typename T >
		inline Vec4 Texture1D( Sampler1D const & p_sampler, T const & p_value )
		{
			return p_sampler.m_writer->Texture1D( p_value );
		}

		template< typename T >
		inline Vec4 texture2D( Sampler2D const & p_sampler, T const & p_value )
		{
			return p_sampler.m_writer->Texture2D( p_value );
		}

		template< typename T >
		inline Vec4 Texture3D( Sampler3D const & p_sampler, T const & p_value )
		{
			return p_sampler.m_writer->Texture3D( p_value );
		}

		namespace
		{
			template< typename T >
			inline Castor::String ToString( T const & p_value )
			{
				return Castor::String( p_value );
			}

			template<>
			inline Castor::String ToString< int >( int const & p_value )
			{
				return Castor::str_utils::to_string( p_value );
			}

			template<>
			inline Castor::String ToString< double >( double const & p_value )
			{
				return Castor::str_utils::to_string( p_value );
			}

			template<>
			inline Castor::String ToString< float >( float const & p_value )
			{
				return Castor::str_utils::to_string( p_value );
			}

			template< typename Vec, typename Value, typename ... Values > void ConstructVec( Castor::String & p_separator, Vec &, Value const &, Values const & ... );

			template< typename Vec >
			inline void ConstructVec( Castor::String & p_separator, Vec & p_vec )
			{
				p_vec.m_result << cuT( "()" );
			}

			template< typename Vec, typename Type >
			inline void ConstructVec( Castor::String & p_separator, Vec & p_vec, Type p_last )
			{
				p_vec.m_result << p_separator << ToString( p_last ) << cuT( " )" );
			}

			template< typename Vec, typename Value, typename ... Values >
			inline void ConstructVec( Castor::String & p_separator, Vec & p_vec, Value const & p_current, Values const & ... p_values )
			{
				p_vec.m_result << p_separator << ToString( p_current );
				p_separator = cuT( ", " );
				ConstructVec( p_separator, p_vec, p_values... );
			}
		}

		template< typename ... Values >
		inline Vec4 vec4( Values const & ... p_values )
		{
			Vec4 l_return;
			l_return.m_result << cuT( "vec4" );
			Castor::String l_separator = cuT( "( " );
			ConstructVec< Vec4, Values... >( l_separator, l_return, p_values... );
			return l_return;
		}

		template< typename ... Values >
		inline Vec3 vec3( Values const & ... p_values )
		{
			Vec4 l_return;
			l_return.m_result << cuT( "vec4" );
			Castor::String l_separator = cuT( "( " );
			ConstructVec< Vec3, Values... >( l_separator, l_return, p_values... );
			return l_return;
		}
	}
}

#endif
