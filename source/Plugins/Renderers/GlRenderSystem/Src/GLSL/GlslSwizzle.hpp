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
#ifndef ___GLSL_SWIZZLE_H___
#define ___GLSL_SWIZZLE_H___

#include "GlslBaseTypes.hpp"

#define GL_WRITER_USE_SWIZZLE 0

#if defined( RGB )
#	undef RGB
#endif

namespace GlRender
{
	namespace GLSL
	{
		template< typename Input, typename Output >
		struct Swizzle
				: public Expr
		{
			inline Swizzle( Castor::String const & p_name, Input * p_input );
			inline Swizzle & operator=( Swizzle const & p_rhs );
			template< typename T > inline Swizzle & operator=( T const & p_rhs );
			inline operator Output()const;
			template< typename UInput, typename UOutput > inline Swizzle & operator+=( Swizzle< UInput, UOutput > const & p_type );
			template< typename UInput, typename UOutput > inline Swizzle & operator-=( Swizzle< UInput, UOutput > const & p_type );
			template< typename UInput, typename UOutput > inline Swizzle & operator*=( Swizzle< UInput, UOutput > const & p_type );
			template< typename UInput, typename UOutput > inline Swizzle & operator/=( Swizzle< UInput, UOutput > const & p_type );
			inline Swizzle & operator+=( float p_type );
			inline Swizzle & operator-=( float p_type );
			inline Swizzle & operator*=( float p_type );
			inline Swizzle & operator/=( float p_type );
			inline Swizzle & operator+=( int p_type );
			inline Swizzle & operator-=( int p_type );
			inline Swizzle & operator*=( int p_type );
			inline Swizzle & operator/=( int p_type );

			Input * m_input;
		};

		template< typename TInput, typename UInput, typename Output > inline Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Output > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Float > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator+( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator-( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator*( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b );
		template< typename TInput, typename UInput, typename Output > inline Output operator/( Swizzle< TInput, Output > const & p_a, Swizzle< UInput, Int > const & p_b );
		template< typename Input, typename Output > inline Output operator+( Swizzle< Input, Output > const & p_a, float p_b );
		template< typename Input, typename Output > inline Output operator-( Swizzle< Input, Output > const & p_a, float p_b );
		template< typename Input, typename Output > inline Output operator*( Swizzle< Input, Output > const & p_a, float p_b );
		template< typename Input, typename Output > inline Output operator/( Swizzle< Input, Output > const & p_a, float p_b );
		template< typename Input, typename Output > inline Output operator+( Swizzle< Input, Output > const & p_a, int p_b );
		template< typename Input, typename Output > inline Output operator-( Swizzle< Input, Output > const & p_a, int p_b );
		template< typename Input, typename Output > inline Output operator*( Swizzle< Input, Output > const & p_a, int p_b );
		template< typename Input, typename Output > inline Output operator/( Swizzle< Input, Output > const & p_a, int p_b );

#if GL_WRITER_USE_SWIZZLE

#	define GLSL_SWIZZLE( Input, Output, Name )\
		Swizzle< Input, Output > Name;

#	define GLSL_FIRST_SWIZZLE( Input, Output, Name )\
		Swizzle< Input, Output > Name = Swizzle< Input, Output >( Castor::string::string_cast< xchar >( #Name ), this )

#	define GLSL_LAST_SWIZZLE( Input, Output, Name )\
		Swizzle< Input, Output > Name = Swizzle< Input, Output >( Castor::string::string_cast< xchar >( #Name ), this )

#	define X x
#	define Y y
#	define Z z
#	define W w
#	define R r
#	define G g
#	define B b
#	define A a
#	define XY xy
#	define XZ xz
#	define XW xw
#	define YX yx
#	define YZ yz
#	define YW yw
#	define ZX zx
#	define ZY zy
#	define ZW zw
#	define WX wx
#	define WY wy
#	define WZ wz
#	define RG rg
#	define RB rb
#	define RA ra
#	define GR gr
#	define GB gb
#	define GA ga
#	define BR br
#	define BG bg
#	define BA ba
#	define AR ar
#	define AG ag
#	define AB ab
#	define XYZ xyz
#	define XYW xyw
#	define XZY xzy
#	define XZW xzw
#	define XWY xwy
#	define XWZ xwz
#	define YXZ yxz
#	define YXW yxw
#	define YZX yzx
#	define YZW yzw
#	define YWX ywx
#	define YWZ ywz
#	define ZXY zxy
#	define ZXW zxw
#	define ZYX zyx
#	define ZYW zyw
#	define ZWX zwx
#	define ZWY zwy
#	define WXY wxy
#	define WXZ wxz
#	define WYX wyx
#	define WYZ wyz
#	define WZX wzx
#	define WZY wzy
#	define RGB rgb
#	define RGA rga
#	define RBG rbg
#	define RBA rba
#	define RAG rag
#	define RAB rab
#	define GRB grb
#	define GRA gra
#	define GBR gbr
#	define GBA gba
#	define GAR gar
#	define GAB gab
#	define BRG brg
#	define BRA bra
#	define BGR bgr
#	define BGA bga
#	define BAR bar
#	define BAG bag
#	define ARG arg
#	define ARB arb
#	define AGR agr
#	define AGB agb
#	define ABR abr
#	define ABG abg
#	define XYZW xyzw
#	define XYWZ xywz
#	define XZYW xzyw
#	define XZWY xzwy
#	define XWYZ xwyz
#	define XWZY xwzy
#	define YXZW yxzw
#	define YXWZ yxwz
#	define YZXW yzxw
#	define YZWX yzwx
#	define YWXZ ywxz
#	define YWZX ywzx
#	define ZXYW zxyw
#	define ZXWY zxwy
#	define ZYXW zyxw
#	define ZYWX zywx
#	define ZWXY zwxy
#	define ZWYX zwyx
#	define WXYZ wxyz
#	define WXZY wxzy
#	define WYXZ wyxz
#	define WYZX wyzx
#	define WZXY wzxy
#	define WZYX wzyx
#	define RGBA rgba
#	define RGAB rgab
#	define RBGA rbga
#	define RBAG rbag
#	define RAGB ragb
#	define RABG rabg
#	define GRBA grba
#	define GRAB grab
#	define GBRA gbra
#	define GBAR gbar
#	define GARB garb
#	define GABR gabr
#	define BRGA brga
#	define BRAG brag
#	define BGRA bgra
#	define BGAR bgar
#	define BARG barg
#	define BAGR bagr
#	define ARGB argb
#	define ARBG arbg
#	define AGRB agrb
#	define AGBR agbr
#	define ABRG abrg
#	define ABGR abgr

#else

#	define GLSL_SWIZZLE( Input, Output, Name )\
		inline Output Name()const\
		{\
			Castor::String l_me( *this );\
			Output l_return( m_writer, l_me );\
			l_return.m_value << l_me << cuT( "."#Name );\
			return l_return;\
		}

#	define GLSL_FIRST_SWIZZLE( Input, Output, Name )\
		GLSL_SWIZZLE( Input, Output, Name )

#	define GLSL_LAST_SWIZZLE( Input, Output, Name )\
		GLSL_SWIZZLE( Input, Output, Name )


#	define X x()
#	define Y y()
#	define Z z()
#	define W w()
#	define R r()
#	define G g()
#	define B b()
#	define A a()
#	define XY xy()
#	define XZ xz()
#	define XW xw()
#	define YX yx()
#	define YZ yz()
#	define YW yw()
#	define ZX zx()
#	define ZY zy()
#	define ZW zw()
#	define WX wx()
#	define WY wy()
#	define WZ wz()
#	define RG rg()
#	define RB rb()
#	define RA ra()
#	define GR gr()
#	define GB gb()
#	define GA ga()
#	define BR br()
#	define BG bg()
#	define BA ba()
#	define AR ar()
#	define AG ag()
#	define AB ab()
#	define XYZ xyz()
#	define XYW xyw()
#	define XZY xzy()
#	define XZW xzw()
#	define XWY xwy()
#	define XWZ xwz()
#	define YXZ yxz()
#	define YXW yxw()
#	define YZX yzx()
#	define YZW yzw()
#	define YWX ywx()
#	define YWZ ywz()
#	define ZXY zxy()
#	define ZXW zxw()
#	define ZYX zyx()
#	define ZYW zyw()
#	define ZWX zwx()
#	define ZWY zwy()
#	define WXY wxy()
#	define WXZ wxz()
#	define WYX wyx()
#	define WYZ wyz()
#	define WZX wzx()
#	define WZY wzy()
#	define RGB rgb()
#	define RGA rga()
#	define RBG rbg()
#	define RBA rba()
#	define RAG rag()
#	define RAB rab()
#	define GRB grb()
#	define GRA gra()
#	define GBR gbr()
#	define GBA gba()
#	define GAR gar()
#	define GAB gab()
#	define BRG brg()
#	define BRA bra()
#	define BGR bgr()
#	define BGA bga()
#	define BAR bar()
#	define BAG bag()
#	define ARG arg()
#	define ARB arb()
#	define AGR agr()
#	define AGB agb()
#	define ABR abr()
#	define ABG abg()
#	define XYZW xyzw()
#	define XYWZ xywz()
#	define XZYW xzyw()
#	define XZWY xzwy()
#	define XWYZ xwyz()
#	define XWZY xwzy()
#	define YXZW yxzw()
#	define YXWZ yxwz()
#	define YZXW yzxw()
#	define YZWX yzwx()
#	define YWXZ ywxz()
#	define YWZX ywzx()
#	define ZXYW zxyw()
#	define ZXWY zxwy()
#	define ZYXW zyxw()
#	define ZYWX zywx()
#	define ZWXY zwxy()
#	define ZWYX zwyx()
#	define WXYZ wxyz()
#	define WXZY wxzy()
#	define WYXZ wyxz()
#	define WYZX wyzx()
#	define WZXY wzxy()
#	define WZYX wzyx()
#	define RGBA rgba()
#	define RGAB rgab()
#	define RBGA rbga()
#	define RBAG rbag()
#	define RAGB ragb()
#	define RABG rabg()
#	define GRBA grba()
#	define GRAB grab()
#	define GBRA gbra()
#	define GBAR gbar()
#	define GARB garb()
#	define GABR gabr()
#	define BRGA brga()
#	define BRAG brag()
#	define BGRA bgra()
#	define BGAR bgar()
#	define BARG barg()
#	define BAGR bagr()
#	define ARGB argb()
#	define ARBG arbg()
#	define AGRB agrb()
#	define AGBR agbr()
#	define ABRG abrg()
#	define ABGR abgr()

#endif
	}
}

#include "GlslSwizzle.inl"

#endif
