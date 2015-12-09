#include "GlOverlayRenderer.hpp"

#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include "GlslSource.hpp"

#include <Engine.hpp>
#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

GlOverlayRenderer::GlOverlayRenderer( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
	: OverlayRenderer( p_renderSystem )
	, Holder( p_gl )
{
}

GlOverlayRenderer::~GlOverlayRenderer()
{
}

ShaderProgramBaseSPtr GlOverlayRenderer::DoCreateProgram( uint32_t p_flags )
{
	using namespace GLSL;

	// Shader program
	ShaderManager & l_manager = GetOwner()->GetOwner()->GetShaderManager();
	ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
	l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
	l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );

	// Vertex shader
	String l_strVs;
	{
		GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		UBO_MATRIX( l_writer );

		// Shader inputs
		ATTRIBUTE( l_writer, IVec2, vertex );
		ATTRIBUTE( l_writer, Vec2, texture );

		// Shader outputs
		OUT( l_writer, Vec2, vtx_texture );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.X, vertex.Y, 0.0, 1.0 );
		} );

		l_strVs = l_writer.Finalise();
	}

	// Pixel shader
	String l_strPs;
	{
		// Vertex shader
		GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		UBO_PASS( l_writer );

		// Shader inputs
		IN( l_writer, Vec2, vtx_texture );
		UNIFORM( l_writer, Sampler2D, c3d_mapText );
		UNIFORM( l_writer, Sampler2D, c3d_mapColour );
		UNIFORM( l_writer, Sampler2D, c3d_mapOpacity );

		// Shader outputs
		LAYOUT( l_writer, Vec4, pxl_v4FragColor );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, c3d_v4MatAmbient );
			LOCALE_ASSIGN( l_writer, Float,  l_fAlpha, c3d_fMatOpacity );

			if ( ( p_flags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
			{
				l_fAlpha *= texture2D( c3d_mapText, vec2( vtx_texture.X, vtx_texture.Y ) ).R;
			}

			if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_v4Ambient = texture2D( c3d_mapColour, vec2( vtx_texture.X, vtx_texture.Y ) );
			}

			if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_fAlpha *= texture2D( c3d_mapOpacity, vec2( vtx_texture.X, vtx_texture.Y ) ).R;
			}

			pxl_v4FragColor = vec4( l_v4Ambient.XYZ, l_fAlpha );
		} );

		l_strPs = l_writer.Finalise();
	}

	if ( ( p_flags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
	{
		l_program->CreateFrameVariable( ShaderProgramBase::MapText, eSHADER_TYPE_PIXEL );
	}

	if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
	{
		l_program->CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
	}

	if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
	{
		l_program->CreateFrameVariable( ShaderProgramBase::MapOpacity, eSHADER_TYPE_PIXEL );
	}

	for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
	{
		l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL( i ), l_strVs );
		l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL( i ), l_strPs );
	}

	return l_program;
}

void GlOverlayRenderer::DoInitialise()
{
}

void GlOverlayRenderer::DoCleanup()
{
}

//*************************************************************************************************
