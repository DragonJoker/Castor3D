#include "OpaquePass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	OpaquePass::OpaquePass( RenderTarget & p_renderTarget
		, Castor3D::RenderTechnique & p_technique )
		: Castor3D::RenderTechniquePass{ cuT( "deferred_opaque" )
			, p_renderTarget
			, p_technique
			, true
			, false }
		, m_directionalShadowMap{ *p_renderTarget.GetEngine() }
		, m_spotShadowMap{ *p_renderTarget.GetEngine() }
		, m_pointShadowMap{ *p_renderTarget.GetEngine() }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	bool OpaquePass::InitialiseShadowMaps()
	{
		auto & l_scene = *m_target.GetScene();
		l_scene.GetLightCache().ForEach( [&l_scene, this]( Light & p_light )
		{
			if ( p_light.IsShadowProducer() )
			{
				switch ( p_light.GetLightType() )
				{
				case LightType::eDirectional:
					m_directionalShadowMap.AddLight( p_light );
					break;

				case LightType::ePoint:
					m_pointShadowMap.AddLight( p_light );
					break;

				case LightType::eSpot:
					m_spotShadowMap.AddLight( p_light );
					break;
				}
			}
		} );

		bool l_result = m_directionalShadowMap.Initialise();

		if ( l_result )
		{
			l_result = m_spotShadowMap.Initialise();
		}

		if ( l_result )
		{
			l_result = m_pointShadowMap.Initialise();
		}

		ENSURE( l_result );
		return l_result;
	}

	void OpaquePass::CleanupShadowMaps()
	{
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
	}

	void OpaquePass::UpdateShadowMaps( RenderQueueArray & p_queues )
	{
		m_pointShadowMap.Update( *m_target.GetCamera(), p_queues );
		m_spotShadowMap.Update( *m_target.GetCamera(), p_queues );
		m_directionalShadowMap.Update( *m_target.GetCamera(), p_queues );
	}

	void OpaquePass::RenderShadowMaps()
	{
	}

	void OpaquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
	}

	void OpaquePass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_sceneFlags, SceneFlag::eShadowFilterStratifiedPoisson );
	}

	String OpaquePass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );
		UBO_MODEL( l_writer );

		// Fragment Inputs
		auto vtx_position = l_writer.GetInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_tangentSpaceFragPosition = l_writer.GetInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" ) );
		auto vtx_tangentSpaceViewPosition = l_writer.GetInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" ) );
		auto vtx_normal = l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );

		auto c3d_fheightScale( l_writer.GetUniform< Float >( cuT( "c3d_fheightScale" ), CheckFlag( p_textureFlags, TextureChannel::eHeight ), 0.1_f ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t l_index = 0;
		auto out_c3dPosition = l_writer.GetFragData< Vec4 >( cuT( "out_c3dPosition" ), l_index++ );
		auto out_c3dDiffuse = l_writer.GetFragData< Vec4 >( cuT( "out_c3dDiffuse" ), l_index++ );
		auto out_c3dNormal = l_writer.GetFragData< Vec4 >( cuT( "out_c3dNormal" ), l_index++ );
		auto out_c3dSpecular = l_writer.GetFragData< Vec4 >( cuT( "out_c3dSpecular" ), l_index++ );
		auto out_c3dEmissive = l_writer.GetFragData< Vec4 >( cuT( "out_c3dEmissive" ), l_index++ );

		auto l_parallaxMapping = DeclareParallaxMappingFunc( l_writer, p_textureFlags, p_programFlags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vtx_normal ) );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), c3d_v4MatDiffuse.xyz() );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), c3d_v4MatSpecular.xyz() );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), vtx_position );
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), vtx_texture );

			if ( CheckFlag( p_textureFlags, TextureChannel::eHeight )
				&& CheckFlag( p_textureFlags, TextureChannel::eNormal ) )
			{
				auto l_viewDir = -l_writer.GetLocale( cuT( "l_viewDir" ), normalize( vtx_tangentSpaceFragPosition - vtx_tangentSpaceViewPosition ) );
				l_texCoord.xy() = l_parallaxMapping( l_texCoord.xy(), l_viewDir );
			}

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );
			ComputePostLightingMapContributions( l_writer, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );
			
			auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_position, 1.0 ) ).xyz() );
			out_c3dPosition = vec4( l_v3Position, l_writer.Cast< Float >( c3d_iShadowReceiver ) );
			out_c3dDiffuse = vec4( l_v3Diffuse, 0.0_f );
			out_c3dNormal = vec4( l_v3Normal, 0.0_f );
			out_c3dSpecular = vec4( l_v3Specular, l_fMatShininess );
			out_c3dEmissive = vec4( l_v3Emissive, 0.0_f );
		} );

		return l_writer.Finalise();
	}

	void OpaquePass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
		auto & l_camera = *m_target.GetCamera();
		auto & l_scene = *l_camera.GetScene();
		auto & l_fog = l_scene.GetFog();
		m_sceneNode.m_fogType.SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_sceneNode.m_fogDensity.SetValue( l_fog.GetDensity() );
		}

		m_sceneNode.m_ambientLight.SetValue( rgba_float( l_scene.GetAmbientLight() ) );
		m_sceneNode.m_backgroundColour.SetValue( rgba_float( l_scene.GetBackgroundColour() ) );
		m_sceneNode.m_cameraPos.SetValue( l_camera.GetParent()->GetDerivedPosition() );
		m_sceneNode.m_sceneUbo.Update();
	}
}
