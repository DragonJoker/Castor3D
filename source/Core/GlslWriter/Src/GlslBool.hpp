/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_BOOL_H___
#define ___GLSL_BOOL_H___

#include "GlslOptional.hpp"

namespace glsl
{
	struct Boolean
		: public Type
	{
		inline Boolean();
		inline Boolean( bool value );
		inline Boolean( GlslWriter * writer
			, castor::String const & name = castor::String() );
		template< typename T >
		inline Boolean & operator=( T const & rhs );
		inline Boolean & operator=( int rhs );
		inline operator uint32_t();
	};

	GlslWriter_API Boolean operator==( Type const & lhs, Type const & rhs );
	GlslWriter_API Boolean operator!=( Type const & lhs, Type const & rhs );
	GlslWriter_API Boolean operator||( Boolean const & lhs, Boolean const & rhs );
	GlslWriter_API Boolean operator&&( Boolean const & lhs, Boolean const & rhs );
}

#include "GlslBool.inl"

#endif
