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

namespace castor3d
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
			shader::BitonicSort bitonic{ writer };

			C3D_Clusters( writer
				, BindingPoints::eClusters
				, 0u
				, &config );
			C3D_LightClusterIndex( writer
				, BindingPoints::eClusterIndex
				, 0u );
			C3D_LightClusterGrid( writer
				, BindingPoints::eClusterGrid
				, 0u );

			writer.implementMainT< sdw::VoidT >( shader::BitonicSort::NumThreads
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
						, min( sdw::UInt{ shader::BitonicSort::BlockSize }, clusterLights.y() ) );

					bitonic.sortT( writer, startOffset, lightCount
						, c3d_lightClusterIndex, c3d_lightClusterIndex
						, in.localInvocationIndex, sdw::UInt{ 0xFFFFFFFFU } );
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: private castor::DataHolderT< ShaderModule >
			, private castor::DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
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
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, castor::String{ cuT( "SortAssigned/" ) } + getName( lightType ), createShader( device, clusters.getConfig() ) } }
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

	crg::FramePass const & createSortAssignedLightsPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, FrustumClusters const & clusters )
	{
		// Point lights
		auto & point = graph.createPass( "SortAssigned/Point"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< sort::FramePass >( framePass
					, context
					, graph
					, device
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z )
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return clusters.getConfig().enablePostAssignSort && !clusters.getCamera().getScene()->getLightCache().getLights( LightType::ePoint ).empty(); } ) )
					, clusters
					, LightType::ePoint );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		point.addDependency( *previousPasses.front() );
		clusters.getClustersUbo().createPassBinding( point, uint32_t( sort::BindingPoints::eClusters ) );
		createInOutStoragePassBinding( point, uint32_t( sort::BindingPoints::eClusterIndex ), cuT( "C3D_PointLightClusterIndex" ), clusters.getPointLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( point, uint32_t( sort::BindingPoints::eClusterGrid ), cuT( "C3D_PointLightClusterGrid" ), clusters.getPointLightClusterGridBuffer(), 0u, ashes::WholeSize );

		// Spot lights
		auto & spot = graph.createPass( "SortAssigned/Spot"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< sort::FramePass >( framePass
					, context
					, graph
					, device
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z )
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return clusters.getConfig().enablePostAssignSort && !clusters.getCamera().getScene()->getLightCache().getLights( LightType::eSpot ).empty(); } ) )
					, clusters
					, LightType::eSpot );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		spot.addDependency( point );
		spot.addDependency( *previousPasses.back() );
		clusters.getClustersUbo().createPassBinding( spot, uint32_t( sort::BindingPoints::eClusters ) );
		createInOutStoragePassBinding( spot, uint32_t( sort::BindingPoints::eClusterIndex ), cuT( "C3D_SpotLightClusterIndex" ), clusters.getSpotLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( spot, uint32_t( sort::BindingPoints::eClusterGrid ), cuT( "C3D_SpotLightClusterGrid" ), clusters.getSpotLightClusterGridBuffer(), 0u, ashes::WholeSize );

		return spot;
	}

	//*********************************************************************************************
}
