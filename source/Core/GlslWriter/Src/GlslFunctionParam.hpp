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
#ifndef ___GLSL_FUNCTION_PARAM_H___
#define ___GLSL_FUNCTION_PARAM_H___

#include "GlslPerVertex.hpp"

namespace glsl
{
	template< typename TypeT >
	struct InParam
		: public TypeT
	{
		InParam( GlslWriter * writer
			, castor::String const & name );
		InParam( TypeT const & other );
		template< typename T >
		inline InParam< TypeT > operator=( T const & rhs );
	};

	template< typename TypeT >
	struct OutParam
		: public TypeT
	{
		OutParam( GlslWriter * writer
			, castor::String const & name );
		OutParam( TypeT const & other );
		template< typename T >
		inline OutParam< TypeT > operator=( T const & rhs );
	};

	template< typename TypeT >
	struct InOutParam
		: public TypeT
	{
		InOutParam( GlslWriter * writer
			, castor::String const & name );
		InOutParam( TypeT const & other );
		template< typename T >
		inline InOutParam< TypeT > operator=( T const & rhs );
	};

	template< typename TypeT >
	struct InArrayParam
		: public Array< TypeT >
	{
		InArrayParam( GlslWriter * writer
			, castor::String const & name
			, uint32_t count );
		InArrayParam( Array< TypeT > const & other );
		template< typename T >
		inline InArrayParam< TypeT > operator=( T const & rhs );
	};

	template< typename TypeT >
	struct OutArrayParam
		: public Array< TypeT >
	{
		OutArrayParam( GlslWriter * writer
			, castor::String const & name
			, uint32_t count );
		OutArrayParam( Array< TypeT > const & other );
		template< typename T >
		inline OutArrayParam< TypeT > operator=( T const & rhs );
	};

	template< typename TypeT >
	struct InOutArrayParam
		: public Array< TypeT >
	{
		InOutArrayParam( GlslWriter * writer
			, castor::String const & name
			, uint32_t count );
		InOutArrayParam( Array< TypeT > const & other );
		template< typename T >
		inline InOutArrayParam< TypeT > operator=( T const & rhs );
	};
}

#include "GlslFunctionParam.inl"

#endif
