#include "MsaaRenderTechnique.hpp"

#include <FrameBuffer.hpp>
#include <RasteriserState.hpp>
#include <RenderTarget.hpp>
#include <ColourRenderBuffer.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <RenderBufferAttachment.hpp>
#include <TextureAttachment.hpp>
#include <DepthStencilStateManager.hpp>
#include <RasteriserStateManager.hpp>
#include <Engine.hpp>
#include <Parameter.hpp>
#include <ShaderProgram.hpp>
#include <DynamicTexture.hpp>

#include <Image.hpp>
#include <Logger.hpp>

#if C3D_HAS_GL_RENDERER
#	include <GlslSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;
#endif

using namespace Castor;
using namespace Castor3D;

namespace Msaa
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "msaa" ), p_renderTarget, p_renderSystem, p_params )
		, m_samplesCount( 0 )
	{
		String l_count;

		if ( p_params.Get( cuT( "samples_count" ), l_count ) )
		{
			m_samplesCount = string::to_int( l_count );
		}

		Logger::LogInfo( StringStream() << cuT( "Using MSAA, " ) << m_samplesCount << cuT( " samples" ) );
		m_msFrameBuffer = m_renderSystem->CreateFrameBuffer();
		m_pMsColorBuffer = m_msFrameBuffer->CreateColourRenderBuffer( m_renderTarget->GetPixelFormat() );
		m_pMsDepthBuffer = m_msFrameBuffer->CreateDepthStencilRenderBuffer( m_renderTarget->GetDepthFormat() );
		m_pMsColorAttach = m_msFrameBuffer->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach = m_msFrameBuffer->CreateAttachment( m_pMsDepthBuffer );
		RasteriserStateSPtr l_pRasteriser = GetEngine()->GetRasteriserStateManager().Create( cuT( "MSAA_RT" ) );
		l_pRasteriser->SetMultisample( true );
		m_wpMsRasteriserState = l_pRasteriser;
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_bReturn = m_msFrameBuffer->Create( m_samplesCount );
		m_pMsColorBuffer->SetSamplesCount( m_samplesCount );
		m_pMsDepthBuffer->SetSamplesCount( m_samplesCount );
		l_bReturn &= m_pMsColorBuffer->Create();
		l_bReturn &= m_pMsDepthBuffer->Create();
		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pMsColorBuffer->Destroy();
		m_pMsDepthBuffer->Destroy();
		m_msFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;
		m_rect = Castor::Rectangle( Position(), m_size );

		if ( l_bReturn )
		{
			l_bReturn = m_pMsColorBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_pMsDepthBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msFrameBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if ( l_bReturn )
			{
				l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, m_pMsColorAttach );

				if ( l_bReturn )
				{
					l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_pMsDepthAttach );
				}

				m_msFrameBuffer->Unbind();
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_msFrameBuffer->DetachAll();
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->Cleanup();
		m_pMsColorBuffer->Cleanup();
		m_pMsDepthBuffer->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();

		if ( !m_samplesCount )
		{
			m_renderTarget->GetRasteriserState()->Apply();
		}
		else
		{
			m_wpMsRasteriserState.lock()->Apply();
		}

		return RenderTechniqueBase::DoRender( m_size, p_scene, p_camera, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->BlitInto( m_renderTarget->GetFrameBuffer(), m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
		m_renderTarget->GetFrameBuffer()->Bind();
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_flags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlPixelShaderSource( p_flags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

#if C3D_HAS_GL_RENDERER

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_flags )const
	{
#define CHECK_FLAG( channel ) ( ( p_flags & ( channel ) ) == ( channel ) )

		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
		l_writer << GLSL::Version() << Endl();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		Vec3 vtx_vertex( l_writer.GetIn< Vec3 >( cuT( "vtx_vertex" ) ) );
		Vec3 vtx_normal( l_writer.GetIn< Vec3 >( cuT( "vtx_normal" ) ) );
		Vec3 vtx_tangent( l_writer.GetIn< Vec3 >( cuT( "vtx_tangent" ) ) );
		Vec3 vtx_bitangent( l_writer.GetIn< Vec3 >( cuT( "vtx_bitangent" ) ) );
		Vec3 vtx_texture( l_writer.GetIn< Vec3 >( cuT( "vtx_texture" ) ) );

		Vec4 pxl_v4FragColor( l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 ) );

		if ( l_writer.GetOpenGl().HasTbo() )
		{
			UNIFORM( l_writer, SamplerBuffer, c3d_sLights );
		}
		else
		{
			UNIFORM( l_writer, Sampler1D, c3d_sLights );
		}

		Optional< Sampler2D > c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) ) );
		Optional< Sampler2D > c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) ) );
		Optional< Sampler2D > c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) ) );
		Optional< Sampler2D > c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) ) );
		Optional< Sampler2D > c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) ) );
		Optional< Sampler2D > c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) ) );
		Optional< Sampler2D > c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CHECK_FLAG( eTEXTURE_CHANNEL_EMISSIVE ) ) );
		Optional< Sampler2D > c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) ) );
		Optional< Sampler2D > c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.X, vtx_normal.Y, vtx_normal.Z ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( 0.0f ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.X, c3d_v3CameraPosition.Y, c3d_v3CameraPosition.Z ) );
			pxl_v4FragColor = vec4( Float( 0.0f ), 0.0f, 0.0f, 0.0f );
			Vec3 l_v3MapNormal( &l_writer, cuT( "l_v3MapNormal" ) );

			if ( p_flags != 0 )
			{
				if ( CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.XY ).XYZ );
					l_v3MapNormal = Float( &l_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &l_writer, 1 ), 1.0, 1.0 );
					l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_EMISSIVE ) )
				{
					l_v3Emissive = texture2D( c3d_mapEmissive, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
				{
					l_fMatShininess = texture2D( c3d_mapGloss, vtx_texture.XY ).R;
				}
			}

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput{ vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput { vtx_vertex, l_v3Normal, vtx_tangent, vtx_bitangent },
											  l_output );
			}
			ROF;

			if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
			{
				l_fAlpha = texture2D( c3d_mapOpacity, vtx_texture.XY ).R * c3d_fMatOpacity;
			}

			if ( p_flags && p_flags != eTEXTURE_CHANNEL_OPACITY )
			{
				if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) )
				{
					l_v3Specular *= texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ;
				}
			}

			pxl_v4FragColor = vec4( l_writer.Paren( l_v3Ambient + c3d_v4MatAmbient.XYZ ) +
									l_writer.Paren( l_v3Diffuse * c3d_v4MatDiffuse.XYZ ) +
									l_writer.Paren( l_v3Specular * c3d_v4MatSpecular.XYZ ) +
									l_v3Emissive, l_fAlpha );
		} );

		return l_writer.Finalise();

#undef CHECK_FLAG
	}

#endif

}
