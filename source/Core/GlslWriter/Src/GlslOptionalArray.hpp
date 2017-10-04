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
#ifndef ___GLSL_OPTIONAL_ARRAY_H___
#define ___GLSL_OPTIONAL_ARRAY_H___

#include "GlslArray.hpp"

namespace glsl
{
	template< typename TypeT >
	struct Optional< Array< TypeT > >
		: public Array< TypeT >
	{
		Optional( GlslWriter * writer
			, castor::String const & name
			, uint32_t dimension
			, bool enabled );
		Optional( Array< TypeT > const & other, bool enabled );
		template< typename T >
		Optional( castor::String const & name
			, uint32_t dimension
			, T const & rhs
			, bool enabled );

		inline Optional< Array< TypeT > > operator=( Optional< Array< TypeT > > const & rhs );
		template< typename T >
		inline Optional< Array< TypeT > > operator=( T const & rhs );
		inline bool isEnabled()const;

		inline operator Optional< Array< Type > >()const;

	private:
		bool m_enabled;
	};

	template< SamplerType ST >
	struct Optional< Array< SamplerT< ST > > >
		: public Array< SamplerT< ST > >
	{
		Optional( GlslWriter * writer
			, castor::String const & name
			, uint32_t dimension
			, bool enabled );
		Optional( GlslWriter * writer
			, uint32_t binding
			, castor::String const & name
			, uint32_t dimension
			, bool enabled );
		Optional( Array< SamplerT< ST > > const & other, bool enabled );
		template< typename T >
		Optional( castor::String const & name
			, uint32_t dimension
			, T const & rhs
			, bool enabled );

		inline Optional< Array< SamplerT< ST > > > operator=( Optional< Array< SamplerT< ST > > > const & rhs );
		template< typename T >
		inline Optional< Array< SamplerT< ST > > > operator=( T const & rhs );
		inline bool isEnabled()const;

		inline operator Optional< Array< Type > >()const;

	private:
		bool m_enabled;
	};

	template< typename TypeT >
	inline castor::String paramToString( castor::String & p_sep, Optional< Array< TypeT > > const & p_value );
}

#include "GlslOptionalArray.inl"

#endif
