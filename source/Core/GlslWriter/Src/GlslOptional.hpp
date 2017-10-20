/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_OPTIONAL_H___
#define ___GLSL_OPTIONAL_H___

#include "GlslVoid.hpp"

namespace glsl
{
	template< typename TypeT >
	struct Optional
		: public TypeT
	{
		Optional( GlslWriter * writer, castor::String const & name, bool enabled );
		Optional( TypeT const & other, bool enabled );
		template< typename T >
		Optional( castor::String const & name, T const & rhs, bool enabled );

		inline Optional< TypeT > operator=( Optional< TypeT > const & rhs );
		template< typename T >
		inline Optional< TypeT > operator=( T const & rhs );
		inline bool isEnabled()const;

		inline operator Optional< Type >()const;

	private:
		bool m_enabled;
	};

	template< SamplerType ST >
	struct Optional< SamplerT< ST > >
		: public SamplerT< ST >
	{
		Optional( GlslWriter * writer, uint32_t binding, castor::String const & name, bool enabled );
		Optional( SamplerT< ST > const & other, bool enabled );
		template< typename T >
		Optional( castor::String const & name, T const & rhs, bool enabled );

		inline Optional< SamplerT< ST > > operator=( Optional< SamplerT< ST > > const & rhs );
		template< typename T >
		inline Optional< SamplerT< ST > > operator=( T const & rhs );
		inline bool isEnabled()const;

		inline operator Optional< Type >()const;

	private:
		bool m_enabled;
	};

	template< typename TypeT >
	inline castor::String paramToString( castor::String & p_sep, Optional< TypeT > const & p_value );

	template< typename T >
	struct is_optional
		: public std::false_type
	{
	};

	template< typename T >
	struct is_optional< Optional< T > >
		: public std::true_type
	{
	};
}

#include "GlslOptional.inl"

#endif
