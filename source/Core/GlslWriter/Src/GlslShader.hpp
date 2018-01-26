/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Shader_H___
#define ___GLSL_Shader_H___

#include "GlslSsbo.hpp"
#include "GlslUbo.hpp"
#include "GlslPcb.hpp"

namespace glsl
{
	struct UniformInfo
	{
		TypeName m_type;
		uint32_t m_count;
	};

	struct SamplerInfo
	{
		TypeName m_type;
		uint32_t m_binding;
		uint32_t m_set;
		uint32_t m_count;
	};

	class Shader
	{
	public:
		inline void registerSsbo( castor::String const & name
			, Ssbo::Info const & info )
		{
			m_ssbos.emplace( name, info );
		}

		inline void registerUbo( castor::String const & name
			, Ubo::Info const & info )
		{
			m_ubos.emplace( name, info );
		}

		inline void registerConstant( castor::String const & name
			, TypeName type )
		{
			m_constants.emplace( name, type );
		}

		inline void registerSampler( castor::String const & name
			, TypeName type
			, uint32_t binding
			, uint32_t set
			, uint32_t count )
		{
			m_samplers.emplace( name, SamplerInfo{ type, binding, set, count } );
		}

		inline void registerUniform( castor::String const & name
			, TypeName type
			, uint32_t count )
		{
			m_uniforms.emplace( name, UniformInfo{ type, count } );
		}

		inline void registerAttribute( castor::String const & name
			, TypeName type )
		{
			m_inputs.emplace( name, type );
		}

		inline void registerInput( castor::String const & name
			, TypeName type )
		{
			m_inputs.emplace( name, type );
		}

		inline void registerOutput( castor::String const & name
			, TypeName type )
		{
			m_outputs.emplace( name, type );
		}

		inline void setSource( castor::String const & src )
		{
			m_source = src;
		}

		inline Ssbo::Info const & getSsboInfo( castor::String const & name )const
		{
			return m_ssbos.at( name );
		}

		inline Ubo::Info const & getUboInfo( castor::String const & name )const
		{
			return m_ubos.at( name );
		}

		inline TypeName getInputType( castor::String const & name )const
		{
			return m_inputs.at( name );
		}

		inline TypeName getOutputType( castor::String const & name )const
		{
			return m_outputs.at( name );
		}
		
		inline TypeName getUniformType( castor::String const & name )const
		{
			return m_uniforms.at( name ).m_type;
		}

		inline castor::String const & getSource()const
		{
			return m_source;
		}

		inline std::map< castor::String, Ubo::Info > const & getUbos()const
		{
			return m_ubos;
		}

		inline std::map< castor::String, Ssbo::Info > const & getSsbos()const
		{
			return m_ssbos;
		}

		inline std::map< castor::String, UniformInfo > const & getUniforms()const
		{
			return m_uniforms;
		}

		inline std::map< castor::String, SamplerInfo > const & getSamplers()const
		{
			return m_samplers;
		}

	private:
		castor::String m_source;
		std::map< castor::String, Ssbo::Info > m_ssbos;
		std::map< castor::String, Ubo::Info > m_ubos;
		std::map< castor::String, TypeName > m_constants;
		std::map< castor::String, SamplerInfo > m_samplers;
		std::map< castor::String, UniformInfo > m_uniforms;
		std::map< castor::String, TypeName > m_inputs;
		std::map< castor::String, TypeName > m_outputs;
	};
}

#endif
