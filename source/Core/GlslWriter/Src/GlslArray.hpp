/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_ARRAY_H___
#define ___GLSL_ARRAY_H___

#include "GlslBool.hpp"

namespace glsl
{
	template< typename T >
	struct Array
		: public T
	{
		Array( GlslWriter * writer
			, const castor::String & name
			, uint32_t dimension );
		template< typename U > T operator[]( U const & offset );
		template< typename U > T operator[]( U const & offset )const;
		uint32_t m_dimension;
	};

	template< SamplerType ST >
	struct Array< SamplerT< ST > >
		: public SamplerT< ST >
	{
		Array( GlslWriter * writer
			, const castor::String & name
			, uint32_t dimension );
		Array( GlslWriter * writer
			, uint32_t binding
			, const castor::String & name
			, uint32_t dimension );
		template< typename U > SamplerT< ST > operator[]( U const & offset );
		template< typename U > SamplerT< ST > operator[]( U const & offset )const;
		uint32_t m_dimension;
	};
}

#include "GlslArray.inl"

#endif
