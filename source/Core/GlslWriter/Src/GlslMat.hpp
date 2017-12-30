/*
See LICENSE file in root folder
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
