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
#ifndef ___GLSL_SWIZZLE_H___
#define ___GLSL_SWIZZLE_H___

#include "GlslBaseTypes.hpp"

#define GL_WRITER_USE_SWIZZLE 0

#if defined( RGB )
#	undef RGB
#endif

namespace GLSL
{
	template< typename Input, typename Output >
	struct Swizzle
		: public Expr
	{
		inline Swizzle( castor::String const & p_name, Input * p_input );
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
	Swizzle< Input, Output > Name = Swizzle< Input, Output >( castor::string::stringCast< xchar >( #Name ), this )

#	define GLSL_LAST_SWIZZLE( Input, Output, Name )\
	Swizzle< Input, Output > Name = Swizzle< Input, Output >( castor::string::stringCast< xchar >( #Name ), this )

#	define SWIZZLE_X x
#	define SWIZZLE_Y y
#	define SWIZZLE_Z z
#	define SWIZZLE_W w
#	define SWIZZLE_R r
#	define SWIZZLE_G g
#	define SWIZZLE_B b
#	define SWIZZLE_A a
#	define SWIZZLE_XY xy
#	define SWIZZLE_XZ xz
#	define SWIZZLE_XW xw
#	define SWIZZLE_YX yx
#	define SWIZZLE_YZ yz
#	define SWIZZLE_YW yw
#	define SWIZZLE_ZX zx
#	define SWIZZLE_ZY zy
#	define SWIZZLE_ZW zw
#	define SWIZZLE_WX wx
#	define SWIZZLE_WY wy
#	define SWIZZLE_WZ wz
#	define SWIZZLE_RG rg
#	define SWIZZLE_RB rb
#	define SWIZZLE_RA ra
#	define SWIZZLE_GR gr
#	define SWIZZLE_GB gb
#	define SWIZZLE_GA ga
#	define SWIZZLE_BR br
#	define SWIZZLE_BG bg
#	define SWIZZLE_BA ba
#	define SWIZZLE_AR ar
#	define SWIZZLE_AG ag
#	define SWIZZLE_AB ab
#	define SWIZZLE_XYZ xyz
#	define SWIZZLE_XYW xyw
#	define SWIZZLE_XZY xzy
#	define SWIZZLE_XZW xzw
#	define SWIZZLE_XWY xwy
#	define SWIZZLE_XWZ xwz
#	define SWIZZLE_YXZ yxz
#	define SWIZZLE_YXW yxw
#	define SWIZZLE_YZX yzx
#	define SWIZZLE_YZW yzw
#	define SWIZZLE_YWX ywx
#	define SWIZZLE_YWZ ywz
#	define SWIZZLE_ZXY zxy
#	define SWIZZLE_ZXW zxw
#	define SWIZZLE_ZYX zyx
#	define SWIZZLE_ZYW zyw
#	define SWIZZLE_ZWX zwx
#	define SWIZZLE_ZWY zwy
#	define SWIZZLE_WXY wxy
#	define SWIZZLE_WXZ wxz
#	define SWIZZLE_WYX wyx
#	define SWIZZLE_WYZ wyz
#	define SWIZZLE_WZX wzx
#	define SWIZZLE_WZY wzy
#	define SWIZZLE_RGB rgb
#	define SWIZZLE_RGA rga
#	define SWIZZLE_RBG rbg
#	define SWIZZLE_RBA rba
#	define SWIZZLE_RAG rag
#	define SWIZZLE_RAB rab
#	define SWIZZLE_GRB grb
#	define SWIZZLE_GRA gra
#	define SWIZZLE_GBR gbr
#	define SWIZZLE_GBA gba
#	define SWIZZLE_GAR gar
#	define SWIZZLE_GAB gab
#	define SWIZZLE_BRG brg
#	define SWIZZLE_BRA bra
#	define SWIZZLE_BGR bgr
#	define SWIZZLE_BGA bga
#	define SWIZZLE_BAR bar
#	define SWIZZLE_BAG bag
#	define SWIZZLE_ARG arg
#	define SWIZZLE_ARB arb
#	define SWIZZLE_AGR agr
#	define SWIZZLE_AGB agb
#	define SWIZZLE_ABR abr
#	define SWIZZLE_ABG abg
#	define SWIZZLE_XYZW xyzw
#	define SWIZZLE_XYWW xyww
#	define SWIZZLE_XYWZ xywz
#	define SWIZZLE_XZYW xzyw
#	define SWIZZLE_XZWY xzwy
#	define SWIZZLE_XWYZ xwyz
#	define SWIZZLE_XWZY xwzy
#	define SWIZZLE_YXZW yxzw
#	define SWIZZLE_YXWZ yxwz
#	define SWIZZLE_YZXW yzxw
#	define SWIZZLE_YZWX yzwx
#	define SWIZZLE_YWXZ ywxz
#	define SWIZZLE_YWZX ywzx
#	define SWIZZLE_ZXYW zxyw
#	define SWIZZLE_ZXWY zxwy
#	define SWIZZLE_ZYXW zyxw
#	define SWIZZLE_ZYWX zywx
#	define SWIZZLE_ZWXY zwxy
#	define SWIZZLE_ZWYX zwyx
#	define SWIZZLE_WXYZ wxyz
#	define SWIZZLE_WXZY wxzy
#	define SWIZZLE_WYXZ wyxz
#	define SWIZZLE_WYZX wyzx
#	define SWIZZLE_WZXY wzxy
#	define SWIZZLE_WZYX wzyx
#	define SWIZZLE_RGBA rgba
#	define SWIZZLE_RGAB rgab
#	define SWIZZLE_RBGA rbga
#	define SWIZZLE_RBAG rbag
#	define SWIZZLE_RAGB ragb
#	define SWIZZLE_RABG rabg
#	define SWIZZLE_GRBA grba
#	define SWIZZLE_GRAB grab
#	define SWIZZLE_GBRA gbra
#	define SWIZZLE_GBAR gbar
#	define SWIZZLE_GARB garb
#	define SWIZZLE_GABR gabr
#	define SWIZZLE_BRGA brga
#	define SWIZZLE_BRAG brag
#	define SWIZZLE_BGRA bgra
#	define SWIZZLE_BGAR bgar
#	define SWIZZLE_BARG barg
#	define SWIZZLE_BAGR bagr
#	define SWIZZLE_ARGB argb
#	define SWIZZLE_ARBG arbg
#	define SWIZZLE_AGRB agrb
#	define SWIZZLE_AGBR agbr
#	define SWIZZLE_ABRG abrg
#	define SWIZZLE_ABGR abgr

#else

#	define GLSL_SWIZZLE( Input, Output, Name )\
	inline Output Name()const\
	{\
		castor::String me( *this );\
		Output result( m_writer, me );\
		result.m_value << me << cuT( "."#Name );\
		return result;\
	}

#	define GLSL_FIRST_SWIZZLE( Input, Output, Name )\
	GLSL_SWIZZLE( Input, Output, Name )

#	define GLSL_LAST_SWIZZLE( Input, Output, Name )\
	GLSL_SWIZZLE( Input, Output, Name )


#	define SWIZZLE_X x()
#	define SWIZZLE_Y y()
#	define SWIZZLE_Z z()
#	define SWIZZLE_W w()
#	define SWIZZLE_R r()
#	define SWIZZLE_G g()
#	define SWIZZLE_B b()
#	define SWIZZLE_A a()
#	define SWIZZLE_XY xy()
#	define SWIZZLE_XZ xz()
#	define SWIZZLE_XW xw()
#	define SWIZZLE_YX yx()
#	define SWIZZLE_YZ yz()
#	define SWIZZLE_YW yw()
#	define SWIZZLE_ZX zx()
#	define SWIZZLE_ZY zy()
#	define SWIZZLE_ZW zw()
#	define SWIZZLE_WX wx()
#	define SWIZZLE_WY wy()
#	define SWIZZLE_WZ wz()
#	define SWIZZLE_RG rg()
#	define SWIZZLE_RB rb()
#	define SWIZZLE_RA ra()
#	define SWIZZLE_GR gr()
#	define SWIZZLE_GB gb()
#	define SWIZZLE_GA ga()
#	define SWIZZLE_BR br()
#	define SWIZZLE_BG bg()
#	define SWIZZLE_BA ba()
#	define SWIZZLE_AR ar()
#	define SWIZZLE_AG ag()
#	define SWIZZLE_AB ab()
#	define SWIZZLE_XYZ xyz()
#	define SWIZZLE_XYW xyw()
#	define SWIZZLE_XZY xzy()
#	define SWIZZLE_XZW xzw()
#	define SWIZZLE_XWY xwy()
#	define SWIZZLE_XWZ xwz()
#	define SWIZZLE_YXZ yxz()
#	define SWIZZLE_YXW yxw()
#	define SWIZZLE_YZX yzx()
#	define SWIZZLE_YZW yzw()
#	define SWIZZLE_YWX ywx()
#	define SWIZZLE_YWZ ywz()
#	define SWIZZLE_ZXY zxy()
#	define SWIZZLE_ZXW zxw()
#	define SWIZZLE_ZYX zyx()
#	define SWIZZLE_ZYW zyw()
#	define SWIZZLE_ZWX zwx()
#	define SWIZZLE_ZWY zwy()
#	define SWIZZLE_WXY wxy()
#	define SWIZZLE_WXZ wxz()
#	define SWIZZLE_WYX wyx()
#	define SWIZZLE_WYZ wyz()
#	define SWIZZLE_WZX wzx()
#	define SWIZZLE_WZY wzy()
#	define SWIZZLE_RGB rgb()
#	define SWIZZLE_RGA rga()
#	define SWIZZLE_RBG rbg()
#	define SWIZZLE_RBA rba()
#	define SWIZZLE_RAG rag()
#	define SWIZZLE_RAB rab()
#	define SWIZZLE_GRB grb()
#	define SWIZZLE_GRA gra()
#	define SWIZZLE_GBR gbr()
#	define SWIZZLE_GBA gba()
#	define SWIZZLE_GAR gar()
#	define SWIZZLE_GAB gab()
#	define SWIZZLE_BRG brg()
#	define SWIZZLE_BRA bra()
#	define SWIZZLE_BGR bgr()
#	define SWIZZLE_BGA bga()
#	define SWIZZLE_BAR bar()
#	define SWIZZLE_BAG bag()
#	define SWIZZLE_ARG arg()
#	define SWIZZLE_ARB arb()
#	define SWIZZLE_AGR agr()
#	define SWIZZLE_AGB agb()
#	define SWIZZLE_ABR abr()
#	define SWIZZLE_ABG abg()
#	define SWIZZLE_XYZW xyzw()
#	define SWIZZLE_XYWW xyww()
#	define SWIZZLE_XYWZ xywz()
#	define SWIZZLE_XZYW xzyw()
#	define SWIZZLE_XZWY xzwy()
#	define SWIZZLE_XWYZ xwyz()
#	define SWIZZLE_XWZY xwzy()
#	define SWIZZLE_YXZW yxzw()
#	define SWIZZLE_YXWZ yxwz()
#	define SWIZZLE_YZXW yzxw()
#	define SWIZZLE_YZWX yzwx()
#	define SWIZZLE_YWXZ ywxz()
#	define SWIZZLE_YWZX ywzx()
#	define SWIZZLE_ZXYW zxyw()
#	define SWIZZLE_ZXWY zxwy()
#	define SWIZZLE_ZYXW zyxw()
#	define SWIZZLE_ZYWX zywx()
#	define SWIZZLE_ZWXY zwxy()
#	define SWIZZLE_ZWYX zwyx()
#	define SWIZZLE_WXYZ wxyz()
#	define SWIZZLE_WXZY wxzy()
#	define SWIZZLE_WYXZ wyxz()
#	define SWIZZLE_WYZX wyzx()
#	define SWIZZLE_WZXY wzxy()
#	define SWIZZLE_WZYX wzyx()
#	define SWIZZLE_RGBA rgba()
#	define SWIZZLE_RGAB rgab()
#	define SWIZZLE_RBGA rbga()
#	define SWIZZLE_RBAG rbag()
#	define SWIZZLE_RAGB ragb()
#	define SWIZZLE_RABG rabg()
#	define SWIZZLE_GRBA grba()
#	define SWIZZLE_GRAB grab()
#	define SWIZZLE_GBRA gbra()
#	define SWIZZLE_GBAR gbar()
#	define SWIZZLE_GARB garb()
#	define SWIZZLE_GABR gabr()
#	define SWIZZLE_BRGA brga()
#	define SWIZZLE_BRAG brag()
#	define SWIZZLE_BGRA bgra()
#	define SWIZZLE_BGAR bgar()
#	define SWIZZLE_BARG barg()
#	define SWIZZLE_BAGR bagr()
#	define SWIZZLE_ARGB argb()
#	define SWIZZLE_ARBG arbg()
#	define SWIZZLE_AGRB agrb()
#	define SWIZZLE_AGBR agbr()
#	define SWIZZLE_ABRG abrg()
#	define SWIZZLE_ABGR abgr()

#endif
}

#include "GlslSwizzle.inl"

#endif
