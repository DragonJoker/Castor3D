#include "ReinhardToneMapping.hpp"

#include <Engine.hpp>
#include <ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;
using namespace Castor3D;

namespace Reinhard
{
	namespace
	{
		String const Gamma = cuT( "c3d_gamma" );
	}

	String ToneMapping::Name = cuT( "reinhard" );

	ToneMapping::ToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: Castor3D::ToneMapping{ Name, p_engine, p_parameters }
		, m_gamma{ 1.0f }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_gamma = string::to_float( l_param );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( p_engine, p_parameters );
	}

	String ToneMapping::DoCreate( FrameVariableBuffer & p_ubo )
	{
		p_ubo.CreateVariable( FrameVariableType::Float, Gamma );
		p_ubo.GetVariable( Gamma, m_gammaVar );

		String l_pxl;
		{
			auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo l_config{ l_writer, ToneMapping::HdrConfig };
			auto c3d_exposure = l_config.GetUniform< Float >( ToneMapping::Exposure );
			auto c3d_gamma = l_config.GetUniform< Float >( Gamma );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_hdrColor, texture( c3d_mapDiffuse, vtx_texture ).SWIZZLE_RGB );
				// Exposure tone mapping
				LOCALE_ASSIGN( l_writer, Vec3, l_mapped, vec3( Float( 1.0f ) ) - exp( -l_hdrColor * c3d_exposure ) );
				// Gamma correction
				l_mapped = pow( l_mapped, vec3( 1.0f / c3d_gamma ) );
				plx_v4FragColor = vec4( l_mapped, 1.0 );
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
		m_gammaVar->SetValue( m_gamma );
	}

	bool ToneMapping::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Gamma " ) + string::to_string( m_gamma ) ) > 0;
	}
}
