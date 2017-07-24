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

	void DepthPass::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		RemFlag( programFlags, ProgramFlag::eLighting );
		RemFlag( programFlags, ProgramFlag::eEnvironmentMapping );
	}

	void DepthPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )const
	{
		if ( !p_nodes.m_staticNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedStaticNodes.m_backCulled.empty()
			|| !p_nodes.m_skinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_instantiatedSkinnedNodes.m_backCulled.empty()
			|| !p_nodes.m_morphingNodes.m_backCulled.empty()
			|| !p_nodes.m_billboardNodes.m_backCulled.empty() )
		{
			m_matrixUbo.Update( p_camera.GetView()
				, p_camera.GetViewport().GetProjection() );
			DoRender( p_nodes.m_instantiatedStaticNodes.m_frontCulled, p_camera );
			DoRender( p_nodes.m_staticNodes.m_frontCulled, p_camera );
			DoRender( p_nodes.m_skinnedNodes.m_frontCulled, p_camera );
			DoRender( p_nodes.m_instantiatedSkinnedNodes.m_frontCulled, p_camera );
			DoRender( p_nodes.m_morphingNodes.m_frontCulled, p_camera );
			DoRender( p_nodes.m_billboardNodes.m_frontCulled, p_camera );

			DoRender( p_nodes.m_instantiatedStaticNodes.m_backCulled, p_camera );
			DoRender( p_nodes.m_staticNodes.m_backCulled, p_camera );
			DoRender( p_nodes.m_skinnedNodes.m_backCulled, p_camera );
			DoRender( p_nodes.m_instantiatedSkinnedNodes.m_backCulled, p_camera );
			DoRender( p_nodes.m_morphingNodes.m_backCulled, p_camera );
			DoRender( p_nodes.m_billboardNodes.m_backCulled, p_camera );
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
		auto it = m_backPipelines.find( p_flags );

		if ( it == m_backPipelines.end() )
		{
			DepthStencilState dsState;
			dsState.SetDepthTest( true );
			dsState.SetDepthMask( WritingMask::eAll );
			RasteriserState rsState;
			rsState.SetCulledFaces( Culling::eNone );
			auto & pipeline = *m_backPipelines.emplace( p_flags
				, GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState )
					, std::move( rsState )
					, BlendState{}
					, MultisampleState{}
					, p_program
					, p_flags ) ).first->second;

			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
				, [this, &pipeline, p_flags]()
			{
				pipeline.AddUniformBuffer( m_matrixUbo.GetUbo() );
				pipeline.AddUniformBuffer( m_modelMatrixUbo.GetUbo() );
				pipeline.AddUniformBuffer( m_modelUbo.GetUbo() );

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
				{
					pipeline.AddUniformBuffer( m_billboardUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
					&& !CheckFlag( p_flags.m_programFlags, ProgramFlag::eInstantiation ) )
				{
					pipeline.AddUniformBuffer( m_skinningUbo.GetUbo() );
				}

				if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
				{
					pipeline.AddUniformBuffer( m_morphingUbo.GetUbo() );
				}
			} ) );
		}
	}

	GLSL::Shader DepthPass::DoGetVertexShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		using namespace GLSL;
		auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();
		// Vertex inputs
		auto position = writer.DeclAttribute< Vec4 >( ShaderProgram::Position );
		auto bone_ids0 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.DeclAttribute< IVec4 >( ShaderProgram::BoneIds1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights0, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.DeclAttribute< Vec4 >( ShaderProgram::Weights1, CheckFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.DeclAttribute< Mat4 >( ShaderProgram::Transform, CheckFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.DeclAttribute< Vec4 >( ShaderProgram::Position2, CheckFlag( programFlags, ProgramFlag::eMorphing ) );

		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		SkinningUbo::Declare( writer, programFlags );
		UBO_MORPHING( writer, programFlags );
		UBO_MODEL( writer );

		// Outputs
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		std::function< void() > main = [&]()
		{
			auto v4Vertex = writer.DeclLocale( cuT( "v4Vertex" ), vec4( position.xyz(), 1.0 ) );
			auto mtxModel = writer.DeclLocale< Mat4 >( cuT( "mtxModel" ) );

			if ( CheckFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				mtxModel = SkinningUbo::ComputeTransform( writer, programFlags );
			}
			else if ( CheckFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				mtxModel = transform;
			}
			else
			{
				mtxModel = c3d_mtxModel;
			}

			if ( CheckFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				auto time = writer.DeclLocale( cuT( "time" ), 1.0_f - c3d_fTime );
				v4Vertex = vec4( v4Vertex.xyz() * time + position2.xyz() * c3d_fTime, 1.0 );
			}

			v4Vertex = mtxModel * v4Vertex;
			v4Vertex = c3d_mtxView * v4Vertex;
			gl_Position = c3d_mtxProjection * v4Vertex;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}

	GLSL::Shader DepthPass::DoGetGeometryShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return GLSL::Shader{};
	}

	GLSL::Shader DepthPass::DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader DepthPass::DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader DepthPass::DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		return DoGetPixelShaderSource( textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	GLSL::Shader DepthPass::DoGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
		} );

		return writer.Finalise();
	}
}
