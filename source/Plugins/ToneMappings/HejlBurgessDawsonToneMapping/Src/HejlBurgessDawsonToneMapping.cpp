#include "HejlBurgessDawsonToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

using namespace castor;
using namespace castor3d;
using namespace glsl;

namespace HejlBurgessDawson
{
	String ToneMapping::Name = cuT( "hejl" );

	ToneMapping::ToneMapping( Engine & engine, Parameters const & p_parameters )
		: castor3d::ToneMapping{ Name, engine, p_parameters }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( engine, p_parameters );
	}

	glsl::Shader ToneMapping::doCreate()
	{
		glsl::Shader pxl;
		{
			auto writer = getEngine()->getRenderSystem()->createGlslWriter();

			// Shader inputs
			Ubo config{ writer, ToneMapping::HdrConfigUbo, HdrConfigUbo::BindingPoint };
			auto c3d_exposure = config.declMember< Float >( ShaderProgram::Exposure );
			config.end();
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				auto x = writer.declLocale( cuT( "x" ), max( hdrColor - 0.004_f, 0.0_f ) );
				plx_v4FragColor = vec4( writer.paren( x * writer.paren( 6.2f * x + 0.5f ) )
					/ writer.paren( x * writer.paren( 6.2f * x + 1.7f ) + 0.06f ), 1.0 );
			} );

			pxl = writer.finalise();
		}

		return pxl;
	}

	void ToneMapping::doDestroy()
	{
	}

	void ToneMapping::doUpdate()
	{
	}

	bool ToneMapping::doWriteInto( TextFile & p_file )
	{
		return true;
	}
}
