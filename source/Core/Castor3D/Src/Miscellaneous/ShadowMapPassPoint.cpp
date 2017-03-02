#include "ShadowMapPassPoint.hpp"

#include "Engine.hpp"
#include "Cache/SamplerCache.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatricesUbo = cuT( "ShadowMatrices" );
		static String const ShadowMatrices = cuT( "c3d_mtxShadowMatrices" );

		void DoUpdateShadowMatrices( Matrix4x4r const & p_projection
			, Point3r const & p_position
			, UniformBuffer & p_shadowConfig
			, UniformBuffer & p_shadowMatrices )
		{
			std::array< Matrix4x4r, size_t( CubeMapFace::eCount ) > const l_views
			{
				{
					p_projection * matrix::look_at( p_position, p_position + Point3r{ +1, 0, 0 }, Point3r{ 0, -1, 0 } ),// Positive X
					p_projection * matrix::look_at( p_position, p_position + Point3r{ -1, 0, 0 }, Point3r{ 0, -1, 0 } ),// Negative X
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, +1, 0 }, Point3r{ 0, 0, +1 } ),// Positive Y
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, -1, 0 }, Point3r{ 0, 0, -1 } ),// Negative Y
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, 0, +1 }, Point3r{ 0, -1, 0 } ),// Positive Z
					p_projection * matrix::look_at( p_position, p_position + Point3r{ 0, 0, -1 }, Point3r{ 0, -1, 0 } ),// Negative Z
				}
			};

			p_shadowConfig.GetUniform< UniformType::eVec3f >( WorldLightPosition )->SetValue( p_position );
			p_shadowConfig.GetUniform< UniformType::eFloat >( FarPlane )->SetValue( 2000.0f );
			auto l_shadowMatrices = p_shadowMatrices.GetUniform< UniformType::eMat4x4f >( ShadowMatrices );
			uint32_t l_index{ 0 };

			for ( auto & l_view : l_views )
			{
				l_shadowMatrices->SetValue( l_view, l_index++ );
			}
		}
	}

	ShadowMapPassPoint::ShadowMapPassPoint( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
		: ShadowMapPass{ p_engine, p_scene, p_light, p_shadowMap, p_index }
		, m_shadowMatrices{ ShadowMatricesUbo, *p_engine.GetRenderSystem() }
		, m_shadowConfig{ ShadowMapUbo, *p_engine.GetRenderSystem() }
	{
		m_shadowConfig.CreateUniform< UniformType::eVec3f >( WorldLightPosition );
		m_shadowConfig.CreateUniform< UniformType::eFloat >( FarPlane );

		m_shadowMatrices.CreateUniform< UniformType::eMat4x4f >( ShadowMatrices, 6u );

		m_renderQueue.Initialise( m_scene );
	}

	ShadowMapPassPoint::~ShadowMapPassPoint()
	{
	}

	ShadowMapPassSPtr ShadowMapPassPoint::Create( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
	{
		return std::make_shared< ShadowMapPassPoint >( p_engine, p_scene, p_light, p_shadowMap, p_index );
	}

	void ShadowMapPassPoint::DoRenderNodes( SceneRenderNodes & p_nodes )
	{
		auto l_depthMaps = DepthMapArray{};
		DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_backCulled );
		DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_backCulled );
		DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_backCulled );
		DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_backCulled );
		DoRenderBillboards( p_nodes.m_billboardNodes.m_backCulled );
	}

	bool ShadowMapPassPoint::DoInitialisePass( Size const & p_size )
	{
		bool l_return{ false };
		auto l_texture = m_shadowMap.GetTexture();

#if USE_GS_POINT_SHADOW_MAPS

		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->Attach( AttachmentPoint::eDepth, 0, m_depthAttach, l_texture->GetType(), m_index );
		m_frameBuffer->SetDrawBuffers( FrameBuffer::AttachArray{} );
		l_return = m_frameBuffer->IsComplete();
		m_frameBuffer->Unbind();

#else

		m_depthAttachs[0] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::ePositiveX );
		m_depthAttachs[1] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::eNegativeX );
		m_depthAttachs[2] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::ePositiveY );
		m_depthAttachs[3] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::eNegativeY );
		m_depthAttachs[4] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::ePositiveZ );
		m_depthAttachs[5] = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace::eNegativeZ );

		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->SetDrawBuffers( FrameBuffer::AttachArray{} );
		l_return = m_frameBuffer->IsComplete();
		m_frameBuffer->Unbind();

#endif

		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 2000.0_r;
		matrix::perspective( m_projection, Angle::from_degrees( 90.0_r ), l_aspect, l_near, l_far );

		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		return true;
	}

	void ShadowMapPassPoint::DoCleanupPass()
	{
		m_shadowConfig.Cleanup();
		m_shadowMatrices.Cleanup();

#if USE_GS_POINT_SHADOW_MAPS

		m_depthAttach.reset();

#else

		for ( auto & attach : m_depthAttachs )
		{
			attach.reset();
		}

#endif

		auto l_node = m_light.GetParent();
		m_onNodeChanged.disconnect();
		m_shadowMap.Cleanup();
	}

	void ShadowMapPassPoint::DoUpdate( RenderQueueArray & p_queues )
	{
		auto l_position = m_light.GetParent()->GetDerivedPosition();
		m_light.Update( l_position );
		p_queues.push_back( m_renderQueue );
		DoUpdateShadowMatrices( m_projection, l_position, m_shadowConfig, m_shadowMatrices );
	}

	void ShadowMapPassPoint::DoRender()
	{
		if ( m_initialised )
		{
			m_shadowConfig.Update();
			m_shadowMatrices.Update();

#if USE_GS_POINT_SHADOW_MAPS

			m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes();
			DoRenderNodes( l_nodes );
			m_frameBuffer->Unbind();

#else

			for ( auto & attach : m_depthAttachs )
			{
				m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, attach, TextureType::eCube );
				m_frameBuffer->Clear();
				auto & l_nodes = m_renderQueue.GetRenderNodes();
				DoRenderNodes( l_nodes );
				m_frameBuffer->Unbind();
			}

#endif
		}
	}

	void ShadowMapPassPoint::DoUpdateProgram( ShaderProgram & p_program )
	{
	}

	void ShadowMapPassPoint::DoPrepareBackPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
		if ( m_backPipelines.find( p_flags ) == m_backPipelines.end() )
		{
			DoUpdateProgram( p_program );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
			{
				l_pipeline.AddUniformBuffer( m_matrixUbo );
				l_pipeline.AddUniformBuffer( m_modelMatrixUbo );
				l_pipeline.AddUniformBuffer( m_sceneUbo );
				l_pipeline.AddUniformBuffer( m_shadowConfig );
				l_pipeline.AddUniformBuffer( m_shadowMatrices );

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					l_pipeline.AddUniformBuffer( m_billboardUbo );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
				{
					l_pipeline.AddUniformBuffer( m_skinningUbo );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					l_pipeline.AddUniformBuffer( m_morphingUbo );
				}

				m_initialised = true;
			} ) );
		}
	}

	String ShadowMapPassPoint::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Vertex inputs
		auto position = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position );
		auto bone_ids0 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.GetAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.GetAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.GetAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );

		// Outputs
		auto vtx_worldSpacePosition = l_writer.GetOutput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.GetLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_mtxModel = l_writer.GetLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.GetLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
				l_mtxBoneTransform = c3d_mtxBones[bone_ids0[0_i]] * weights0[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[1_i]] * weights0[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[2_i]] * weights0[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids0[3_i]] * weights0[3_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[0_i]] * weights1[0_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[1_i]] * weights1[1_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[2_i]] * weights1[2_i];
				l_mtxBoneTransform += c3d_mtxBones[bone_ids1[3_i]] * weights1[3_i];
				l_mtxModel = c3d_mtxModel * l_mtxBoneTransform;
			}
			else if ( CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) )
			{
				l_mtxModel = transform;
			}
			else
			{
				l_mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( p_programFlags, ProgramFlag::eMorphing ) )
			{
				auto l_time = l_writer.GetLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
			}

			l_v4Vertex = l_mtxModel * l_v4Vertex;
			vtx_worldSpacePosition = l_v4Vertex.xyz();

			gl_Position = l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMapPassPoint::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Geometry layout
		l_writer << InputLayout{ InputLayout::eTriangles };
		l_writer << OutputLayout{ OutputLayout::eTriangleStrip, 18 };

		// Geometry inputs
		Ubo l_shadowMatricesUbo{ l_writer, ShadowMatricesUbo };
		auto c3d_mtxShadowMatrices = l_shadowMatricesUbo.GetUniform< Mat4 >( ShadowMatrices, 6u );
		l_shadowMatricesUbo.End();

		auto gl_in = l_writer.GetBuiltin< gl_PerVertex >( cuT( "gl_in" ), 8u );
		auto gl_Layer = l_writer.GetBuiltin< Int >( cuT( "gl_Layer" ) );

		//Geometry outputs
		auto geo_position = l_writer.GetOutput< Vec4 >( cuT( "geo_position" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			FOR( l_writer, Int, face, 1, cuT( "face < 6" ), cuT( "++face" ) )
			{
				gl_Layer = face; // built-in variable that specifies to which face we render.

				FOR( l_writer, Int, i, 0, cuT( "i < 3" ), cuT(  "++i" ) )
				{
					geo_position = gl_in[i].gl_Position();
					gl_Position = c3d_mtxShadowMatrices[face] * geo_position;
					l_writer.EmitVertex();
					l_writer << Endl();
				}
				ROF;

				l_writer.EndPrimitive();
			}
			ROF;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String ShadowMapPassPoint::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.GetUniform< Float >( FarPlane ) );
		l_shadowMap.End();

		auto geo_position( l_writer.GetInput< Vec4 >( cuT( "geo_position" ) ) );

		// Fragment Outputs
		auto gl_FragDepth( l_writer.GetBuiltin< Float >( cuT( "gl_FragDepth" ) ) );

		auto l_main = [&]()
		{
			auto l_distance = l_writer.GetLocale( cuT( "l_distance" ), length( geo_position.xyz() - c3d_v3WordLightPosition ) );
			gl_FragDepth = l_distance / c3d_fFarPlane;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
