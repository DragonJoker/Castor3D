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
#ifndef ___GLSL_Shader_H___
#define ___GLSL_Shader_H___

#include "GlslSsbo.hpp"
#include "GlslUbo.hpp"

namespace glsl
{
	class Shader
	{
	public:
		inline void registerSsbo( castor::String const & p_name, Ssbo::Info const & p_info )
		{
			m_ssbos.emplace( p_name, p_info );
		}

		inline void registerUbo( castor::String const & p_name, Ubo::Info const & p_info )
		{
			m_ubos.emplace( p_name, p_info );
		}

		inline void registerUniform( castor::String const & p_name, TypeName p_type )
		{
			m_uniforms.emplace( p_name, p_type );
		}

		inline void registerAttribute( castor::String const & p_name, TypeName p_type )
		{
			m_inputs.emplace( p_name, p_type );
		}

		inline void registerInput( castor::String const & p_name, TypeName p_type )
		{
			m_inputs.emplace( p_name, p_type );
		}

		inline void registerOutput( castor::String const & p_name, TypeName p_type )
		{
			m_outputs.emplace( p_name, p_type );
		}

		inline void setSource( castor::String const & p_src )
		{
			m_source = p_src;
		}

		inline Ssbo::Info const & getSsboInfo( castor::String const & p_name )const
		{
			return m_ssbos.at( p_name );
		}

		inline Ubo::Info const & getUboInfo( castor::String const & p_name )const
		{
			return m_ubos.at( p_name );
		}

		inline TypeName getInputType( castor::String const & p_name )const
		{
			return m_inputs.at( p_name );
		}

		inline TypeName getOutputType( castor::String const & p_name )const
		{
			return m_outputs.at( p_name );
		}
		
		inline TypeName getUniformType( castor::String const & p_name )const
		{
			return m_uniforms.at( p_name );
		}

		inline castor::String const & getSource()const
		{
			return m_source;
		}

	private:
		castor::String m_source;
		std::map< castor::String, Ssbo::Info > m_ssbos;
		std::map< castor::String, Ubo::Info > m_ubos;
		std::map< castor::String, TypeName > m_uniforms;
		std::map< castor::String, TypeName > m_inputs;
		std::map< castor::String, TypeName > m_outputs;
	};
}

#endif
