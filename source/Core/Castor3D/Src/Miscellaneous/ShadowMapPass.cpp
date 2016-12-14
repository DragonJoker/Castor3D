#include "ShadowMapPass.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPass::ShadowMapPass( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
		: RenderPass{ cuT( "ShadowMap" ), p_engine, true }
		, m_light{ p_light }
		, m_scene{ p_scene }
		, m_shadowMap{ p_shadowMap }
		, m_index{ p_index }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	void ShadowMapPass::Render()
	{
		DoRender();
	}

	void ShadowMapPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		auto l_depthMaps = DepthMapArray{};
		DoRenderInstancedSubmeshes( p_nodes.m_instancedGeometries.m_backCulled, p_camera, l_depthMaps );
		DoRenderStaticSubmeshes( p_nodes.m_staticGeometries.m_backCulled, p_camera, l_depthMaps );
		DoRenderAnimatedSubmeshes( p_nodes.m_animatedGeometries.m_backCulled, p_camera, l_depthMaps );
		DoRenderBillboards( p_nodes.m_billboards.m_backCulled, p_camera, l_depthMaps );
	}

	bool ShadowMapPass::DoInitialise( Size const & p_size )
	{
		bool l_return = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			l_return = m_frameBuffer->Create();

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( p_size );
			}

			if ( l_return )
			{
				l_return = DoInitialisePass( p_size );
			}
		}

		return l_return;
	}

	void ShadowMapPass::DoCleanup()
	{
		if ( m_frameBuffer )
		{
			DoCleanupPass();

			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}

	void ShadowMapPass::DoUpdatePipeline( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )const
	{
	}

	void ShadowMapPass::DoPrepareFrontPipeline( ShaderProgram & p_program
		, PipelineFlags const & p_flags )
	{
	}

	void ShadowMapPass::DoPrepareBackPipeline( ShaderProgram & p_program
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
			l_pipeline.AddUniformBuffer( m_matrixUbo );
			l_pipeline.AddUniformBuffer( m_sceneUbo );
			l_pipeline.AddUniformBuffer( m_modelUbo );

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eBillboards ) )
			{
				l_pipeline.AddUniformBuffer( m_billboardUbo );
			}

			if ( CheckFlag( p_flags.m_programFlags, ProgramFlag::eSkinning )
				|| CheckFlag( p_flags.m_programFlags, ProgramFlag::eMorphing ) )
			{
				l_pipeline.AddUniformBuffer( m_animationUbo );
			}
		}
	}

	void ShadowMapPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		RemFlag( p_programFlags, ProgramFlag::eLighting );
		RemFlag( p_programFlags, ProgramFlag::eAlphaBlending );
		RemFlag( p_textureFlags, TextureChannel( uint16_t( TextureChannel::eAll ) & ~uint16_t( TextureChannel::eOpacity ) ) );

		AddFlag( p_programFlags, ProgramFlag::eShadowMap );
	}
}
