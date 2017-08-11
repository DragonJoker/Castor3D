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
#ifndef ___GLSL_MAT_H___
#define ___GLSL_MAT_H___

#include "GlslVec.hpp"

namespace glsl
{
	template< typename ValueT >
	struct Mat2T
		: public Type
	{
		using my_type = ValueT;
		using my_vec = Vec2T< ValueT >;
		using my_mat = Mat2T< ValueT >;

		inline Mat2T();
		inline Mat2T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_mat & operator=( my_mat const & p_rhs );
		template< typename RhsT > inline my_mat & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_vec operator[]( IndexT const & p_rhs )const;
		inline my_vec operator[]( int const & p_rhs )const;
	};

	template< typename ValueT >
	struct Mat3T
		: public Type
	{
		using my_type = ValueT;
		using my_vec = Vec3T< ValueT >;
		using my_mat = Mat3T< ValueT >;

		inline Mat3T();
		inline Mat3T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_mat & operator=( my_mat const & p_rhs );
		template< typename RhsT > inline my_mat & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_vec operator[]( IndexT const & p_rhs )const;
		inline my_vec operator[]( int const & p_rhs )const;
	};

	template< typename ValueT >
	struct Mat4T
		: public Type
	{
		using my_type = ValueT;
		using my_vec = Vec4T< ValueT >;
		using my_mat = Mat4T< ValueT >;

		inline Mat4T();
		inline Mat4T( GlslWriter * p_writer, castor::String const & p_name = castor::String() );
		inline my_mat & operator=( my_mat const & p_rhs );
		template< typename RhsT > inline my_mat & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_vec operator[]( IndexT const & p_rhs )const;
		inline my_vec operator[]( int const & p_rhs )const;
	};
}

#include "GlslMat.inl"

#endif
