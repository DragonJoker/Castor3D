#include "Castor3D/Render/Clustered/ClustersMask.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Prepass/DepthPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <limits>

namespace castor3d
{
	//*********************************************************************************************

	namespace clsmsk
	{
		class NodesPass
			: public RenderTechniqueNodesPass
		{
		public:
			NodesPass( RenderTechnique * parent
				, crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::ImageViewIdArray targetDepth
				, SsaoConfig const & ssaoConfig
				, RenderNodesPassDesc const & renderPassDesc )
				: RenderTechniqueNodesPass{ parent
					, pass
					, context
					, graph
					, device
					, cuT( "c3d.clusters_mask" )
					, {}
					, std::move( targetDepth )
					, renderPassDesc
					, { false, ssaoConfig } }
			{
			}

			ShaderFlags getShaderFlags()const override
			{
				return ( ShaderFlag::eOpacity
					| ShaderFlag::eViewSpace
					| ShaderFlag::eDepth );
			}

			bool isPassEnabled()const override
			{
				return m_parent->getClustersConfig()
					&& m_parent->getClustersConfig()->parseDepthBuffer
					&& hasNodes()
					&& getScene().getLightCache().hasClusteredLights();
			}

		private:
			ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override
			{
				return flags;
			}

			SceneFlags doAdjustSceneFlags( SceneFlags flags )const override
			{
				return SceneFlag::eNone;
			}

			void doFillAdditionalBindings( PipelineFlags const & flags
				, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override
			{
				auto index = uint32_t( GlobalBuffersIdx::eCount );
				auto frustumClusters = getTechnique().getRenderTarget().getFrustumClusters();
				CU_Require( frustumClusters );
				bindings.emplace_back( frustumClusters->getClustersUbo().createLayoutBinding( index++ // ClustersUbo
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // LightsAABB
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // ClustersFlags
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}

			void doFillAdditionalDescriptor( PipelineFlags const & flags
				, ashes::WriteDescriptorSetArray & descriptorWrites
				, ShadowMapLightTypeArray const & shadowMaps
				, ShadowBuffer const * shadowBuffer )override
			{
				auto index = uint32_t( GlobalBuffersIdx::eCount );
				auto frustumClusters = getTechnique().getRenderTarget().getFrustumClusters();
				CU_Require( frustumClusters );
				descriptorWrites.emplace_back( frustumClusters->getClustersUbo().getDescriptorWrite( index++ ) );
				bindBuffer( frustumClusters->getReducedLightsAABBBuffer(), descriptorWrites, index );
				bindBuffer( frustumClusters->getClusterFlagsBuffer(), descriptorWrites, index );
			}

			ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override
			{
				return ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL };
			}

			ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override
			{
				return ashes::PipelineColorBlendStateCreateInfo{ 0u
					, VK_FALSE
					, VK_LOGIC_OP_COPY
					, ashes::VkPipelineColorBlendAttachmentStateArray{} };
			}

			void doGetGeometryShaderSource( PipelineFlags const & flags
				, ast::ShaderBuilder & builder )const override
			{
			}

			void doGetPixelShaderSource( PipelineFlags const & flags
				, ast::ShaderBuilder & builder )const override
			{
				sdw::FragmentWriter writer{ builder };
				bool enableTextures = flags.enableTextures();

				shader::Utils utils{ writer };
				shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
					, flags
					, getComponentsMask()
					, utils };

				C3D_ModelsData( writer
					, GlobalBuffersIdx::eModelsData
					, RenderPipeline::eBuffers );
				shader::Materials materials{ writer
					, passShaders
					, uint32_t( GlobalBuffersIdx::eMaterials )
					, RenderPipeline::eBuffers };
				shader::TextureConfigurations textureConfigs{ writer
					, uint32_t( GlobalBuffersIdx::eTexConfigs )
					, RenderPipeline::eBuffers
					, enableTextures };
				shader::TextureAnimations textureAnims{ writer
					, uint32_t( GlobalBuffersIdx::eTexAnims )
					, RenderPipeline::eBuffers
					, enableTextures };

				C3D_Clusters( writer
					, GlobalBuffersIdx::eCount
					, RenderPipeline::eBuffers
					, m_parent->getClustersConfig() );
				C3D_ReducedLightsAABB( writer
					, uint32_t( GlobalBuffersIdx::eCount ) + 1u
					, RenderPipeline::eBuffers );
				C3D_ClusterFlags( writer
					, uint32_t( GlobalBuffersIdx::eCount ) + 2u
					, RenderPipeline::eBuffers );

				auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
					, 0u
					, RenderPipeline::eTextures
					, enableTextures ) );

				sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
				auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
				pcb.end();

				writer.implementMainT< shader::FragmentSurfaceT, sdw::VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
						, passShaders
						, flags }
					, sdw::FragmentOut{ writer }
					, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > in
						, sdw::FragmentOut out )
					{
						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[in.nodeId - 1u] );
						auto material = writer.declLocale( "material"
							, materials.getMaterial( modelData.getMaterialId() ) );
						auto components = writer.declLocale( "components"
							, shader::BlendComponents{ materials
								, material
								, in } );
						materials.blendMaterials( flags
							, textureConfigs
							, textureAnims
							, c3d_maps
							, material
							, modelData.getMaterialId()
							, in.passMultipliers
							, components );
						auto clusterIndex3D = writer.declLocale( "clusterIndex3D"
							, c3d_clustersData.computeClusterIndex3D( in.fragCoord.xy()
								, in.viewPosition.z()
								, c3d_clustersLightsData ) );
						auto clusterIndex1D = writer.declLocale( "clusterIndex1D"
							, c3d_clustersData.computeClusterIndex1D( clusterIndex3D ) );
						c3d_clusterFlags[clusterIndex1D] = 1_u;
					} );
			}
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createClustersMaskPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters
		, RenderTechnique & technique
		, RenderNodesPass *& nodesPass )
	{
		auto & result = graph.createPass( "NodesPass"
			, [&nodesPass, &device, &technique]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< clsmsk::NodesPass >( &technique
					, framePass
					, context
					, runnableGraph
					, device
					, technique.getTargetDepth()
					, technique.getSsaoConfig()
					, RenderNodesPassDesc{ technique.getTargetExtent()
							, technique.getCameraUbo()
							, technique.getSceneUbo()
							, technique.getRenderTarget().getCuller()
							, RenderFilter::eNone
							, false /* isOit */
							, true /* forceTwoSided */ }
						.safeBand( true )
						.meshShading( true )
						.allowClusteredLighting()
						.componentModeFlags( ComponentModeFlag::eOpacity
							| ComponentModeFlag::eGeometry ) );
				nodesPass = res.get();
				nodesPass->getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( previousPass );
		result.addInOutDepthStencilView( technique.getTargetDepth() );

		return result;
	}

	//*********************************************************************************************
}
