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
#ifndef ___GLSL_BASE_TYPES_H___
#define ___GLSL_BASE_TYPES_H___

#include "GlslSampler.hpp"

namespace glsl
{
	struct Int
		: public Type
	{
		inline Int();
		inline Int( Int && p_value );
		inline Int( Int const & p_value );
		inline Int( Type const & p_value );
		inline Int( int p_value );
		inline Int( float p_value );
		inline Int( GlslWriter * p_writer, int p_value );
		inline Int( GlslWriter * p_writer, float p_value );
		inline Int( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline Int & operator=( Int const & p_rhs );
		template< typename T > inline Int & operator=( T const & p_rhs );
		template< typename T > inline Int & operator=( int p_rhs );
		inline explicit operator uint32_t();
		inline Int & operator++();
		inline Int operator++( int );
		inline Int & operator<<=( int i );
		inline Int & operator>>=( int i );
		inline Int & operator<<=( Int const & i );
		inline Int & operator>>=( Int const & i );
		inline Int & operator<<=( UInt const & i );
		inline Int & operator>>=( UInt const & i );
		inline Int & operator&=( int i );
		inline Int & operator|=( int i );
		inline Int & operator&=( Int const & i );
		inline Int & operator|=( Int const & i );
		inline Int & operator&=( UInt const & i );
		inline Int & operator|=( UInt const & i );
	};

	inline Int operator<<( Int const & p_value, int i );
	inline Int operator>>( Int const & p_value, int i );
	inline Int operator<<( Int const & p_value, Int const & i );
	inline Int operator>>( Int const & p_value, Int const & i );
	inline Int operator<<( Int const & p_value, UInt const & i );
	inline Int operator>>( Int const & p_value, UInt const & i );
	inline Int operator&( Int const & p_value, int i );
	inline Int operator|( Int const & p_value, int i );
	inline Int operator&( Int const & p_value, Int const & i );
	inline Int operator|( Int const & p_value, Int const & i );
	inline Int operator&( Int const & p_value, UInt const & i );
	inline Int operator|( Int const & p_value, UInt const & i );

	inline Int operator "" _i( unsigned long long p_value )
	{
		return Int( int( p_value ) );
	}

	struct UInt
		: public Type
	{
		inline UInt();
		inline UInt( UInt && p_value );
		inline UInt( UInt const & p_value );
		inline UInt( Type const & p_value );
		inline UInt( unsigned int p_value );
		inline UInt( float p_value );
		inline UInt( GlslWriter * p_writer, unsigned int p_value );
		inline UInt( GlslWriter * p_writer, float p_value );
		inline UInt( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline UInt & operator=( UInt const & p_rhs );
		template< typename T > inline UInt & operator=( T const & p_rhs );
		template< typename T > inline UInt & operator=( unsigned int p_rhs );
		inline explicit operator uint32_t();
		inline UInt & operator++();
		inline UInt operator++( int );
		inline UInt & operator<<=( int i );
		inline UInt & operator>>=( int i );
		inline UInt & operator<<=( Int const & i );
		inline UInt & operator>>=( Int const & i );
		inline UInt & operator<<=( UInt const & i );
		inline UInt & operator>>=( UInt const & i );
		inline UInt & operator&=( int i );
		inline UInt & operator|=( int i );
		inline UInt & operator&=( Int const & i );
		inline UInt & operator|=( Int const & i );
		inline UInt & operator&=( UInt const & i );
		inline UInt & operator|=( UInt const & i );
	};

	inline UInt operator<<( UInt const & p_value, int i );
	inline UInt operator>>( UInt const & p_value, int i );
	inline UInt operator<<( UInt const & p_value, Int const & i );
	inline UInt operator>>( UInt const & p_value, Int const & i );
	inline UInt operator<<( UInt const & p_value, UInt const & i );
	inline UInt operator>>( UInt const & p_value, UInt const & i );
	inline UInt operator&( UInt const & p_value, int i );
	inline UInt operator|( UInt const & p_value, int i );
	inline UInt operator&( UInt const & p_value, Int const & i );
	inline UInt operator|( UInt const & p_value, Int const & i );
	inline UInt operator&( UInt const & p_value, UInt const & i );
	inline UInt operator|( UInt const & p_value, UInt const & i );

	inline UInt operator "" _ui( unsigned long long p_value )
	{
		return UInt( static_cast< unsigned int >( p_value ) );
	}

	struct Float
		: public Type
	{
		inline Float();
		inline Float( Float && p_value );
		inline Float( Float const & p_value );
		inline Float( Type const & p_value );
		inline Float( int p_value );
		inline Float( float p_value );
		inline Float( double p_value );
		inline Float( GlslWriter * p_writer, int p_value );
		inline Float( GlslWriter * p_writer, float p_value );
		inline Float( GlslWriter * p_writer, double p_value );
		inline Float( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline Float & operator=( Float const & p_rhs );
		template< typename T > inline Float & operator=( T const & p_rhs );
		template< typename T > inline Float & operator=( float p_rhs );
		inline explicit operator float();
	};

	inline Float operator "" _f( long double p_value )
	{
		return Float( double( p_value ) );
	}
}

#include "GlslBaseTypes.inl"

#endif
