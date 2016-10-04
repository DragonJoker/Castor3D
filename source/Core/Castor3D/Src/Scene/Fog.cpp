#include "Fog.hpp"

#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
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

	void Fog::FillShader( FrameVariableBuffer const & p_buffer )const
	{
		OneIntFrameVariableSPtr l_type;
		p_buffer.GetVariable( ShaderProgram::FogType, l_type );

		if ( l_type )
		{
			l_type->SetValue( int( m_type ) );

			if ( m_type != FogType::Disabled )
			{
				OneFloatFrameVariableSPtr l_density;
				p_buffer.GetVariable( ShaderProgram::FogDensity, l_density );

				if ( l_density )
				{
					l_density->SetValue( m_density );
				}
			}
		}
	}
}
