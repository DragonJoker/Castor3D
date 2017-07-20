#include "ReinhardToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/HdrConfigUbo.hpp>
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
		String param;

		if ( p_parameters.Get( cuT( "Gamma" ), param ) )
		{
			m_config.SetGamma( string::to_float( param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( p_engine, p_parameters );
	}

	GLSL::Shader ToneMapping::DoCreate()
	{
		m_gammaVar = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Gamma );

		GLSL::Shader pxl;
		{
			auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, ShaderProgram::BufferHdrConfig, HdrConfigUbo::BindingPoint };
			auto c3d_fExposure = config.DeclMember< Float >( ShaderProgram::Exposure );
			auto c3d_fGamma = config.DeclMember< Float >( ShaderProgram::Gamma );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			GLSL::Utils utils{ writer };
			utils.DeclareApplyGamma();

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.DeclLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				// Exposure tone mapping
				auto mapped = writer.DeclLocale( cuT( "mapped" ), vec3( Float( 1.0f ) ) - exp( -hdrColor * c3d_fExposure ) );
				// Gamma correction
				plx_v4FragColor = vec4( utils.ApplyGamma( c3d_fGamma, mapped ), 1.0 );
			} );

			pxl = writer.Finalise();
		}

		return pxl;
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
