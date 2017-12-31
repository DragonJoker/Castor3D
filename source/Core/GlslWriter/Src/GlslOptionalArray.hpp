/*
See LICENSE file in root folder
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
