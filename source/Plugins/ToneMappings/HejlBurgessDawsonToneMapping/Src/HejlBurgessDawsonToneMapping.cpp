#include "HejlBurgessDawsonToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Shader/HdrConfigUbo.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GLSL;

namespace HejlBurgessDawson
{
	String ToneMapping::Name = cuT( "hejl" );

	ToneMapping::ToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: Castor3D::ToneMapping{ Name, p_engine, p_parameters }
	{
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
		GLSL::Shader pxl;
		{
			auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, ToneMapping::HdrConfigUbo, HdrConfigUbo::BindingPoint };
			auto c3d_exposure = config.DeclMember< Float >( ShaderProgram::Exposure );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.DeclLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				auto x = writer.DeclLocale( cuT( "x" ), max( hdrColor - 0.004_f, 0.0_f ) );
				plx_v4FragColor = vec4( writer.Paren( x * writer.Paren( 6.2f * x + 0.5f ) )
					/ writer.Paren( x * writer.Paren( 6.2f * x + 1.7f ) + 0.06f ), 1.0 );
			} );

			pxl = writer.Finalise();
		}

		return pxl;
	}

	void ToneMapping::DoDestroy()
	{
	}

	void ToneMapping::DoUpdate()
	{
	}

	bool ToneMapping::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
