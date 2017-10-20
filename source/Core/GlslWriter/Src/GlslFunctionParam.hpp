/*
See LICENSE file in root folder
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
