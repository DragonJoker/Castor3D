#include "ReinhardToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace GLSL;
using namespace Castor3D;

namespace Reinhard
{
	String ToneMapping::Name = cuT( "reinhard" );

	ToneMapping::ToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: Castor3D::ToneMapping{ Name, p_engine, p_parameters }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_config.SetGamma( string::to_float( l_param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( p_engine, p_parameters );
	}

	String ToneMapping::DoCreate()
	{
		m_gammaVar = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Gamma );

		String l_pxl;
		{
			auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo l_config{ l_writer, ShaderProgram::BufferHdrConfig };
			auto c3d_fExposure = l_config.GetUniform< Float >( ShaderProgram::Exposure );
			auto c3d_fGamma = l_config.GetUniform< Float >( ShaderProgram::Gamma );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareApplyGamma();

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_hdrColor = l_writer.GetLocale( cuT( "l_hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				// Exposure tone mapping
				auto l_mapped = l_writer.GetLocale( cuT( "l_mapped" ), vec3( Float( 1.0f ) ) - exp( -l_hdrColor * c3d_fExposure ) );
				// Gamma correction
				plx_v4FragColor = vec4( l_utils.ApplyGamma( c3d_fGamma, l_mapped ), 1.0 );
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
	}

	void ToneMapping::DoDestroy()
	{
		m_gammaVar.reset();
	}

	void ToneMapping::DoUpdate()
	{
		m_gammaVar->SetValue( m_config.GetGamma() );
	}

	bool ToneMapping::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Gamma=" ) + string::to_string( m_config.GetGamma() ) ) > 0;
	}
}
