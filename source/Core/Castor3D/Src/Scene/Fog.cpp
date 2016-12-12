#include "Fog.hpp"

#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	Fog::Fog()
	{
	}

	Fog::~Fog()
	{
	}

	void Fog::FillShader( UniformBuffer const & p_buffer )const
	{
		auto l_type = p_buffer.GetUniform< UniformType::eInt >( ShaderProgram::FogType );

		if ( l_type )
		{
			l_type->SetValue( int( m_type ) );

			if ( m_type != FogType::eDisabled )
			{
				auto l_density = p_buffer.GetUniform< UniformType::eFloat >( ShaderProgram::FogDensity );

				if ( l_density )
				{
					l_density->SetValue( m_density );
				}
			}
		}
	}
}
