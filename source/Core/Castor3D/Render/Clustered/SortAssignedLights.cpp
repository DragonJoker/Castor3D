#include "Castor3D/Render/Clustered/SortAssignedLights.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBitonicSort.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace sort
	{
		enum class BindingPoints
		{
			eClusters,
			eClusterIndex,
			eClusterGrid,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };
			shader::BitonicSortT< 4u > bitonic{ writer, 8u };

			C3D_Clusters( writer
				, BindingPoints::eClusters
				, 0u );
			C3D_LightClusterIndex( writer
				, BindingPoints::eClusterIndex
				, 0u );
			C3D_LightClusterGrid( writer
				, BindingPoints::eClusterGrid
				, 0u );

			writer.implementMainT< sdw::VoidT >( bitonic.threadsCount
				, [&]( sdw::ComputeIn const & in )
				{
					auto clusterIndex3D = writer.declLocale( "clusterIndex3D"
						, in.workGroupID );
					auto clusterIndex1D = writer.declLocale( "clusterIndex1D"
						, c3d_clustersData.computeClusterIndex1D( clusterIndex3D ) );
					auto clusterLights = writer.declLocale( "clusterLights"
						, c3d_lightClusterGrid[clusterIndex1D] );
					auto startOffset = writer.declLocale( "startOffset"
						, clusterLights.x() );
					auto lightCount = writer.declLocale( "lightCount"
						, min( sdw::UInt{ shader::BitonicSortT< 4u >::bucketSize }, clusterLights.y() ) );

					bitonic.sortT( writer
						, startOffset, lightCount
						, in.localInvocationIndex, in.globalInvocationID.x()
						, c3d_lightClusterIndex, c3d_lightClusterIndex
						, sdw::UInt{ 0xFFFFFFFFU } );
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: private DataHolderT< ShaderModule >
			, private DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::cp::Config config
				, FrustumClusters const & clusters
				, LightType lightType )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, String{ cuT( "SortAssigned/" ) } + getName( lightType ), createShader( device, clusters.getConfig() ) } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{}
					, config
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) ) }
			{
			}
		};
	}

	//*********************************************************************************************

	void createSortAssignedLightsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, BufferBase & pointLightClusterIndex
		, BufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid )
	{
		{
			// Point lights
			auto & point = graph.createPass( "SortAssigned/Point"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< sort::FramePass >( framePass
						, context
						, runGraph
						, device
						, crg::cp::Config{}
							.groupCountX( clusters.getDimensions()->x )
							.groupCountY( clusters.getDimensions()->y )
							.groupCountZ( clusters.getDimensions()->z )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return !clusters.getCamera().getScene()->getLightCache().getLightInstances( LightType::ePoint ).empty(); } ) )
						, clusters
						, LightType::ePoint );
					device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			clusters.getClustersUbo().createPassBinding( point, uint32_t( sort::BindingPoints::eClusters ) );
			pointLightClusterIndex.setLastAttach( point.addInOutStorage( *pointLightClusterIndex.getLastAttach(), uint32_t( sort::BindingPoints::eClusterIndex ) ) );
			pointLightClusterGrid.setLastAttach( point.addInOutStorage( *pointLightClusterGrid.getLastAttach(), uint32_t( sort::BindingPoints::eClusterGrid ) ) );
		}
		{
			// Spot lights
			auto & spot = graph.createPass( "SortAssigned/Spot"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< sort::FramePass >( framePass
						, context
						, runGraph
						, device
						, crg::cp::Config{}
							.groupCountX( clusters.getDimensions()->x )
							.groupCountY( clusters.getDimensions()->y )
							.groupCountZ( clusters.getDimensions()->z )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return !clusters.getCamera().getScene()->getLightCache().getLightInstances( LightType::eSpot ).empty(); } ) )
						, clusters
						, LightType::eSpot );
					device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			clusters.getClustersUbo().createPassBinding( spot, uint32_t( sort::BindingPoints::eClusters ) );
			spotLightClusterIndex.setLastAttach( spot.addInOutStorage( *spotLightClusterIndex.getLastAttach(), uint32_t( sort::BindingPoints::eClusterIndex ) ) );
			spotLightClusterGrid.setLastAttach( spot.addInOutStorage( *spotLightClusterGrid.getLastAttach(), uint32_t( sort::BindingPoints::eClusterGrid ) ) );
		}
	}

	//*********************************************************************************************
}
