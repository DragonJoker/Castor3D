#include "ShadowMapPass.hpp"

#include "Engine.hpp"
#include "Cache/ShaderCache.hpp"

#include "Event/Frame/FunctorEvent.hpp"
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
#include "ShadowMap/ShadowMap.hpp"
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
	ShadowMapPass::ShadowMapPass( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: RenderPass{ cuT( "ShadowMap" ), p_engine, true }
		, m_shadowMap{ p_shadowMap }
		, m_light{ p_light }
	{
	}

	ShadowMapPass::~ShadowMapPass()
	{
	}

	void ShadowMapPass::Render()
	{
		DoRender();
	}

	void ShadowMapPass::Update( RenderQueueArray & p_queues
		, int32_t p_index )
	{
		m_index = p_index;
		RenderPass::Update( p_queues );
	}

	void ShadowMapPass::DoRenderNodes( SceneRenderNodes & p_nodes
		, Camera const & p_camera )
	{
		DoRenderInstancedSubmeshes( p_nodes.m_instancedNodes.m_backCulled, p_camera );
		DoRenderStaticSubmeshes( p_nodes.m_staticNodes.m_backCulled, p_camera );
		DoRenderSkinningSubmeshes( p_nodes.m_skinningNodes.m_backCulled, p_camera );
		DoRenderMorphingSubmeshes( p_nodes.m_morphingNodes.m_backCulled, p_camera );
		DoRenderBillboards( p_nodes.m_billboardNodes.m_backCulled, p_camera );
	}

	void ShadowMapPass::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		m_shadowMap.UpdateFlags( p_textureFlags, p_programFlags );
	}

	void ShadowMapPass::DoUpdatePipeline( RenderPipeline & p_pipeline )const
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

	String ShadowMapPass::DoGetVertexShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags
		, bool p_invertNormals )const
	{
		return m_shadowMap.GetVertexShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags
			, p_invertNormals );
	}

	String ShadowMapPass::DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return m_shadowMap.GetGeometryShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}

	String ShadowMapPass::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
	{
		return m_shadowMap.GetPixelShaderSource( p_textureFlags
			, p_programFlags
			, p_sceneFlags );
	}
}
