#include "DepthPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	DepthPass::DepthPass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera )
		: Castor3D::RenderPass{ p_name
			, *p_scene.GetEngine()
			, nullptr }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
	{
	}

	DepthPass::DepthPass( String const & p_name
		, Scene & p_scene
		, Camera * p_camera
		, bool p_oit )
		: Castor3D::RenderPass{ p_name
			, *p_scene.GetEngine()
			, p_oit
			, nullptr }
		, m_scene{ p_scene }
		, m_camera{ p_camera }
	{
	}

	DepthPass::~DepthPass()
	{
	}

	void DepthPass::Render()
	{
		DoRenderNodes( m_renderQueue.GetRenderNodes()
			, *m_camera );
	}

	bool DepthPass::DoInitialise( Size const & CU_PARAM_UNUSED( p_size ) )
	{
		if ( m_camera )
		{
			m_renderQueue.Initialise( m_scene, *m_camera );
		}
		else
		{
			m_renderQueue.Initialise( m_scene );
		}

		return true;
	}

	void DepthPass::DoCleanup()
	{
	}

	void DepthPass::DoUpdate( RenderQueueArray & p_queues )
	{
		p_queues.push_back( m_renderQueue );
	}

	void DepthPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eEnvironmentMapping );
	}

	void DepthPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )const
	{
		if ( !p_nodes.m_staticNodes.m_backCulled.empty()
			|| !p_nodes.m_instancedNodes.m_backCulled.empty()
			|| !p_nodes.m_skinningNodes.m_backCulled.empty()
			|| !p_nodes.m_morphingNodes.m_backCulled.empty()
			|| !p_nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );
			DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_frontCulled, p_camera );
			DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_frontCulled, p_camera );
			DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_frontCulled, p_camera );
			DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_frontCulled, p_camera );
			DoRenderBillboards( p_nodes.m_billboardNodes.m_frontCulled, p_camera );

			DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_backCulled, p_camera );
			DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_backCulled, p_camera );
			DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_backCulled, p_camera );
			DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_backCulled, p_camera );
			DoRenderBillboards( p_nodes.m_billboardNodes.m_backCulled, p_camera );
		}
	}

	void DepthPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void DepthPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
	{
	}

	void DepthPass::DoPrepareBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )
	{
		auto l_it = m_backPipelines.find( p_flags );

		if ( l_it == m_backPipelines.end() )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( true );
			l_dsState.SetDepthMask( WritingMask::eAll );
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			auto & l_pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
					, std::move( l_rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &l_pipeline, p_flags]()
			{
				l_pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
				l_pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
				l_pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					l_pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning ) )
				{
					l_pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					l_pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
				}
			} ) );
		}
	}

	String DepthPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, bool p_invertNormals )const
	{
		using namespace GLSL;
		auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto bone_ids0 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = l_writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( p_programFlags, ProgramFlag::eSkinning ) );
		auto transform = l_writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( p_programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = l_writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( p_programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SKINNING( l_writer, p_programFlags );
		UBO_MORPHING( l_writer, p_programFlags );
		UBO_MODEL( l_writer );

		// Outputs
		auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > l_main = [&]()
		{
			auto l_v4Vertex = l_writer.DeclLocale( cuT( "l_v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto l_mtxModel = l_writer.DeclLocale< Mat4 >( cuT( "l_mtxModel" ) );

			if ( CheckFlag( p_programFlags, ProgramFlag::eSkinning ) )
			{
				auto l_mtxBoneTransform = l_writer.DeclLocale< Mat4 >( cuT( "l_mtxBoneTransform" ) );
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
				auto l_time = l_writer.DeclLocale( cuT( "l_time" ), 1.0_f - c3d_fTime );
				l_v4Vertex = vec4( l_v4Vertex.xyz() * l_time + position2.xyz() * c3d_fTime, 1.0 );
			}

			l_v4Vertex = l_mtxModel * l_v4Vertex;
			l_v4Vertex = c3d_mtxView * l_v4Vertex;
			gl_Position = c3d_mtxProjection * l_v4Vertex;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	String DepthPass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		return String{};
	}

	String DepthPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
		} );

		return l_writer.Finalise();
	}
}
