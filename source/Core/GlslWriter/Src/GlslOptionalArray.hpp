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

namespace GLSL
{
	template< typename TypeT >
	struct Optional< Array< TypeT > >
		: public Array< TypeT >
	{
		Optional( GlslWriter * p_writer, castor::String const & p_name, uint32_t p_dimension , bool p_enabled );
		Optional( Array< TypeT > const & p_other, bool p_enabled );
		template< typename T > Optional( castor::String const & p_name, uint32_t p_dimension, T const & p_rhs, bool p_enabled );
		inline Optional< Array< TypeT > > operator=( Optional< Array< TypeT > > const & p_rhs );
		template< typename T > inline Optional< Array< TypeT > > operator=( T const & p_rhs );
		inline bool isEnabled()const;

		inline operator Optional< Array< TypeT > >()const;

	private:
		bool m_enabled;
	};

	template< typename TypeT >
	inline castor::String paramToString( castor::String & p_sep, Optional< Array< TypeT > > const & p_value );
}

#include "GlslOptionalArray.inl"

#endif
