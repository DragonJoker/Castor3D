/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_SAMPLER_H___
#define ___GLSL_SAMPLER_H___

#include "GlslWriter.hpp"

namespace glsl
{
	template< SamplerType ST >
	struct SamplerT
		: public Type
	{
		inline SamplerT();
		inline SamplerT( GlslWriter * writer
			, castor::String const & name = castor::String() );
		inline SamplerT( GlslWriter * writer
			, uint32_t bind
			, castor::String const & name );
		inline SamplerT( GlslWriter * writer
			, uint32_t bind
			, uint32_t set
			, castor::String const & name );
		template< typename T > inline SamplerT & operator=( T const & rhs );
		inline operator uint32_t();

	private:
		uint32_t m_binding{ 0u };
		uint32_t m_set{ 0u };
	};
}

#include "GlslSampler.inl"

#endif
