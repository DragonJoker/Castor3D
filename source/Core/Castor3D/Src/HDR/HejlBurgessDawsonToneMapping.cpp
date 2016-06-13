#include "HejlBurgessDawsonToneMapping.hpp"

#include "Engine.hpp"
#include "ShaderManager.hpp"

#include "Miscellaneous/Parameter.hpp"
#include "Render/Context.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String HejlBurgessDawsonToneMapping::Name = cuT( "hejl" );

	HejlBurgessDawsonToneMapping::HejlBurgessDawsonToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: ToneMapping{ Name, p_engine, p_parameters }
	{
	}

	HejlBurgessDawsonToneMapping::~HejlBurgessDawsonToneMapping()
	{
	}

	ToneMappingSPtr HejlBurgessDawsonToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< HejlBurgessDawsonToneMapping >( p_engine, p_parameters );
	}

	String HejlBurgessDawsonToneMapping::DoCreate()
	{
		String l_pxl;
		{
			auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo l_config = l_writer.GetUbo( ToneMapping::HdrConfig );
			auto c3d_exposure = l_config.GetUniform< Float >( ToneMapping::Exposure );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_hdrColor, texture2D( c3d_mapDiffuse, vtx_texture ).SWIZZLE_RGB );
				l_hdrColor *= vec3( c3d_exposure );
				LOCALE_ASSIGN( l_writer, Vec3, x, max( vec3( Float( 0 ) ), l_hdrColor - vec3( Float( 0.004 ) ) ) );
				plx_v4FragColor = vec4( ( x * l_writer.Paren( 6.2f * x + 0.5f ) ) / l_writer.Paren( x * l_writer.Paren( 6.2f * x + 1.7f ) + 0.06f ), 1.0 );
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
	}

	void HejlBurgessDawsonToneMapping::DoDestroy()
	{
	}

	void HejlBurgessDawsonToneMapping::DoUpdate()
	{
	}

	bool HejlBurgessDawsonToneMapping::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
