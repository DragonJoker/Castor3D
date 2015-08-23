#include "GlOverlayRenderer.hpp"

#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderSource.hpp"
#include "OpenGl.hpp"

#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

GlOverlayRenderer::GlOverlayRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
	:	OverlayRenderer( p_pRenderSystem )
	,	m_gl( p_gl	)
{
}

GlOverlayRenderer::~GlOverlayRenderer()
{
}

ShaderProgramBaseSPtr GlOverlayRenderer::DoGetProgram( uint32_t p_uiFlags )
{
	using namespace GLSL;
	GLSL::VariablesBase * l_pVariables = GLSL::GetVariables( m_gl );
	GLSL::ConstantsBase * l_pConstants = GLSL::GetConstants( m_gl );
	std::unique_ptr< GLSL::KeywordsBase > l_pKeywords = GLSL::GetKeywords( m_gl );

	String l_strVersion = l_pKeywords->GetVersion();
	String l_strAttribute0 = l_pKeywords->GetAttribute( 0 );
	String l_strAttribute1 = l_pKeywords->GetAttribute( 1 );
	String l_strAttribute2 = l_pKeywords->GetAttribute( 2 );
	String l_strAttribute3 = l_pKeywords->GetAttribute( 3 );
	String l_strAttribute4 = l_pKeywords->GetAttribute( 4 );
	String l_strAttribute5 = l_pKeywords->GetAttribute( 5 );
	String l_strAttribute6 = l_pKeywords->GetAttribute( 6 );
	String l_strAttribute7 = l_pKeywords->GetAttribute( 7 );
	String l_strIn = l_pKeywords->GetIn();
	String l_strOut = l_pKeywords->GetOut();

	// Shader program
	ShaderManager & l_manager = m_pRenderSystem->GetEngine()->GetShaderManager();
	ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
	l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
	l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );

	// Vertex shader
	String l_strVs;
	{
		// Vertex shader
		GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		UBO_MATRIX( l_writer );

		// Shader inputs
		ATTRIBUTE( l_writer, IVec4, vertex );
		ATTRIBUTE( l_writer, Vec2, texture );

		// Shader outputs
		OUT( l_writer, Vec2, vtx_texture );

		l_writer.Implement_Function< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), vertex.z(), 1.0 );
		} );

		l_strVs = l_writer.Finalise();
	}
	
	// Pixel shader
	String l_strPs;
	{
		// Vertex shader
		GlslWriter l_writer( m_gl, eSHADER_TYPE_VERTEX );
		l_writer << Version() << Endl();

		UBO_PASS( l_writer );

		// Shader inputs
		IN( l_writer, Vec2, vtx_texture );
		UNIFORM( l_writer, Sampler2D, c3d_mapText );
		UNIFORM( l_writer, Sampler2D, c3d_mapColour );
		UNIFORM( l_writer, Sampler2D, c3d_mapOpacity );

		// Shader outputs
		LAYOUT( l_writer, Vec4, pxl_v4FragColor );

		l_writer.Implement_Function< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, c3d_v4MatAmbient );
			LOCALE_ASSIGN( l_writer, Float,  l_fAlpha, c3d_fMatOpacity );

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
			{
				l_fAlpha *= texture2D( c3d_mapText, vec2( vtx_texture.x(), vtx_texture.y() ) ).r();
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
			{
				l_v4Ambient = texture2D( c3d_mapColour, vec2( vtx_texture.x(), vtx_texture.y() ) );
			}

			if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			{
				l_fAlpha *= texture2D( c3d_mapOpacity, vec2( vtx_texture.x(), vtx_texture.y() ) ).r();
			}

			pxl_v4FragColor = vec4( l_v4Ambient.xyz(), l_fAlpha );
		} );

		l_strPs = l_writer.Finalise();
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
	{
		l_program->CreateFrameVariable( ShaderProgramBase::MapText, eSHADER_TYPE_PIXEL );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
	{
		l_program->CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
	}

	if ( ( p_uiFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
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
