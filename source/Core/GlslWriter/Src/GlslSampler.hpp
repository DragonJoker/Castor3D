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
#ifndef ___GLSL_SAMPLER_H___
#define ___GLSL_SAMPLER_H___

#include "GlslWriter.hpp"

namespace GLSL
{
	struct SamplerBuffer
		: public Type
	{
		inline SamplerBuffer();
		inline SamplerBuffer( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline SamplerBuffer & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler1D
		: public Type
	{
		inline Sampler1D();
		inline Sampler1D( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler1D & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler2D
		: public Type
	{
		inline Sampler2D();
		inline Sampler2D( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler2D & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler3D
		: public Type
	{
		inline Sampler3D();
		inline Sampler3D( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler3D & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct SamplerCube
		: public Type
	{
		inline SamplerCube();
		inline SamplerCube( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline SamplerCube & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler1DArray
		: public Type
	{
		inline Sampler1DArray();
		inline Sampler1DArray( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler1DArray & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler2DArray
		: public Type
	{
		inline Sampler2DArray();
		inline Sampler2DArray( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler2DArray & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct SamplerCubeArray
		: public Type
	{
		inline SamplerCubeArray();
		inline SamplerCubeArray( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline SamplerCubeArray & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler1DShadow
		: public Type
	{
		inline Sampler1DShadow();
		inline Sampler1DShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler1DShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler2DShadow
		: public Type
	{
		inline Sampler2DShadow();
		inline Sampler2DShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler2DShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct SamplerCubeShadow
		: public Type
	{
		inline SamplerCubeShadow();
		inline SamplerCubeShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline SamplerCubeShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler1DArrayShadow
		: public Type
	{
		inline Sampler1DArrayShadow();
		inline Sampler1DArrayShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler1DArrayShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct Sampler2DArrayShadow
		: public Type
	{
		inline Sampler2DArrayShadow();
		inline Sampler2DArrayShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline Sampler2DArrayShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};

	struct SamplerCubeArrayShadow
		: public Type
	{
		inline SamplerCubeArrayShadow();
		inline SamplerCubeArrayShadow( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		template< typename T > inline SamplerCubeArrayShadow & operator=( T const & p_rhs );
		inline operator uint32_t();
	};
}

#include "GlslSampler.inl"

#endif
