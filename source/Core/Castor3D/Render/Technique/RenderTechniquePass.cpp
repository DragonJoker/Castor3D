#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		template< typename MapType >
		void doSortAlpha( MapType & input
			, Camera const & camera
			, RenderPass::DistanceSortedNodeMap & output )
		{
			for ( auto & itPipelines : input )
			{
				for ( auto & renderNode : itPipelines.second )
				{
					Matrix4x4f worldMeshMatrix = renderNode.sceneNode.getDerivedTransformationMatrix().getInverse().transpose();
					Point3f worldCameraPosition = camera.getParent()->getDerivedPosition();
					Point3f meshCameraPosition = worldCameraPosition * worldMeshMatrix;
					renderNode.data.sortByDistance( meshCameraPosition );
					meshCameraPosition -= renderNode.sceneNode.getPosition();
					output.emplace( point::lengthSquared( meshCameraPosition ), makeDistanceNode( renderNode ) );
				}
			}
		}
	}

	//*************************************************************************************************

	void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		writes.push_back( ashes::WriteDescriptorSet
			{
				index++,
				0u,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				{ {
					sampler,
					view,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				} }
			} );
	}

	void bindShadowMaps( PipelineFlags const & pipelineFlags
		, ShadowMapLightTypeArray const & shadowMaps
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index )
	{
		for ( auto i = 0u; i < uint32_t( LightType::eCount ); ++i )
		{
			if ( checkFlag( pipelineFlags.sceneFlags, SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << i ) ) )
			{
				for ( auto & shadowMapRef : shadowMaps[i] )
				{
					auto & shadowMap = shadowMapRef.first.get();
					auto & result = shadowMap.getShadowPassResult();
					CU_Require( result[SmTexture::eNormalLinear].getTexture()->isInitialised() );
					bindTexture( result[SmTexture::eNormalLinear].getTexture()->getDefaultView().getSampledView()
						, result[SmTexture::eNormalLinear].getSampler()->getSampler()
						, writes
						, index );
					CU_Require( result[SmTexture::eVariance].getTexture()->isInitialised() );
					bindTexture( result[SmTexture::eVariance].getTexture()->getDefaultView().getSampledView()
						, result[SmTexture::eVariance].getSampler()->getSampler()
						, writes
						, index );
				}
			}
		}
	}

	//*************************************************************************************************

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ category, name, *culler.getScene().getEngine(), matrixUbo, culler, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
	{
	}

	RenderTechniquePass::RenderTechniquePass( String const & category
		, String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config )
		: RenderPass{ category, name, *culler.getScene().getEngine(), matrixUbo, culler, oit, ignored }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.hasCamera() ? &culler.getCamera() : nullptr }
		, m_sceneNode{}
		, m_environment{ environment }
		, m_ssaoConfig{ config }
	{
	}

	RenderTechniquePass::~RenderTechniquePass()
	{
	}

	void RenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
	}

	void RenderTechniquePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
	}

	void RenderTechniquePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
	}

	void RenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, node.pipeline.getFlags().textures );
	}

	void RenderTechniquePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		node.passNode.fillDescriptor( layout
			, index
			, writes
			, node.pipeline.getFlags().textures );
		node.texDescriptorSet->setBindings( writes );
	}

	void RenderTechniquePass::update( GpuUpdater & updater )
	{
		doUpdateNodes( m_renderQueue.getCulledRenderNodes()
			, updater.jitter
			, updater.info );
	}

	void RenderTechniquePass::doUpdateNodes( SceneCulledRenderNodes & nodes
		, castor::Point2f const & jitter
		, RenderInfo & info )
	{
		if ( nodes.hasNodes() )
		{
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, info );
		}
	}

	void RenderTechniquePass::doUpdateUbos( CpuUpdater & updater )
	{
		m_sceneUbo.cpuUpdate( *updater.camera->getScene()
			, updater.camera.get() );
	}

	bool RenderTechniquePass::doInitialise( RenderDevice const & device
		, Size const & CU_UnusedParam( size ) )
	{
		m_finished = device->createSemaphore( getName() );
		return true;
	}

	void RenderTechniquePass::doCleanup( RenderDevice const & device )
	{
		m_renderQueue.cleanup();
		m_finished.reset();
	}

	void RenderTechniquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		addFlag( flags.programFlags, ProgramFlag::eLighting );

		if ( m_environment )
		{
			addFlag( flags.programFlags, ProgramFlag::eEnvironmentMapping );
		}
	}

	ShaderPtr RenderTechniquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return nullptr;
	}

	void RenderTechniquePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
		m_sceneUbo.cpuUpdate( m_scene, m_camera );
	}

	ashes::PipelineDepthStencilStateCreateInfo RenderTechniquePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, m_mode != RenderMode::eTransparentOnly };
	}

	ashes::PipelineColorBlendStateCreateInfo RenderTechniquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	ashes::VkDescriptorSetLayoutBindingArray RenderTechniquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = getMinTextureIndex();
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
			index += uint32_t( flags.textures.size() );
		}

		return textureBindings;
	}
}
