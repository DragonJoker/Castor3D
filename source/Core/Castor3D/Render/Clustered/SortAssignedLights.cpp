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
		static uint32_t constexpr BlockSize = 1024u;
		static uint32_t constexpr BatchesPerPass = 8u;
		static uint32_t constexpr BatchSize = BlockSize / BatchesPerPass;
		static uint32_t constexpr ValuesPerThread = BatchesPerPass << 1u;
		static uint32_t constexpr NumThreads = BlockSize / ValuesPerThread;

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

			auto gBatchSize = writer.declConstant( "gBatchSize", sdw::UInt{ BatchSize } );
			auto gBatchSizeLog = writer.declGlobal( "gBatchSizeLog", writer.cast< sdw::UInt >( findMSB( gBatchSize ) ) );
			auto gNumThreads = writer.declConstant( "gNumThreads", sdw::UInt{ NumThreads } );
			auto gMaxUInt = writer.declConstant( "gMaxInt", 0xFFFFFFFF_u );

			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", BlockSize );

			auto bitInsert0 = writer.implementFunction< sdw::UInt >( "bitInsert0"
				, [&]( sdw::UInt const & value
					, sdw::UInt const & bit )
				{
					writer.returnStmt( ( ( ( gMaxUInt << bit ) & value ) << 1u ) | ( ~( gMaxUInt << bit ) & value ) );
				}
				, sdw::InUInt{ writer, "value" }
				, sdw::InUInt{ writer, "bit" } );

			auto sortLights = writer.implementFunction< sdw::Void >( "c3d_sortLights"
				, [&]( sdw::UInt const & groupIndex
					, sdw::UInt const & offset
					, sdw::UInt const & numElements )
				{
					// start with simple version, do everything in group shared memory
	
					// we process a power of two number of elements, 
					auto passCount = writer.declLocale( "passCount", 1u + writer.cast< sdw::UInt >( findMSB( numElements - 1u ) ) );
					auto roundedElementCount = writer.declLocale( "roundedElementCount", 1u << passCount );
					auto batchCount = writer.declLocale( "batchCount", ( roundedElementCount + gBatchSize - 1u ) >> gBatchSizeLog );
					// Load data into shared memory. Pad missing values with max ints.
	
					FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
					{
						// each thread loads a pair of values per batch.
						auto i1 = writer.declLocale( "i1", groupIndex + batch * gBatchSize );
						auto i2 = writer.declLocale( "i2", i1 + ( gBatchSize >> 1u ) );
						gsKeys[i1] = writer.ternary( i1 < numElements, c3d_lightClusterIndex[offset + i1], gMaxUInt );
						gsKeys[i2] = writer.ternary( i2 < numElements, c3d_lightClusterIndex[offset + i2], gMaxUInt );
					}
					ROF

					shader::groupMemoryBarrierWithGroupSync( writer );
	
					// Each loop iteration produces blocks of size k that are monotonic (alternatively increasing and decreasing)
					// thus, producing blocks of size 2*k that are bitonic.
					// as a result, the last pass produces a single block sorted in ascending order
					FOR( writer, sdw::UInt, pass, 0_u, pass < passCount, ++pass )
					{
						auto k = writer.declLocale( "k", 1_u << ( pass + 1u ) );
						// Each iteration compares and optionally swap elements in pairs exactly once for each element
						FOR( writer, sdw::UInt, subPass, 0_u, subPass <= pass, ++subPass )
						{
							FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
							{
								auto indexFirst = writer.declLocale( "indexFirst", bitInsert0( groupIndex + batch * gNumThreads, ( pass - subPass ) ) );
								auto indexSecond = writer.declLocale( "indexSecond", indexFirst | ( 1u << ( pass - subPass ) ) );
								auto valFirst = writer.declLocale( "valFirst", gsKeys[indexFirst] );
								auto valSecond = writer.declLocale( "valSecond", gsKeys[indexSecond] );
								shader::groupMemoryBarrierWithGroupSync( writer );

								IF( writer, writer.ternary( ( indexFirst & k ) == 0_u, 1_u, 0_u ) ^ writer.ternary( valFirst <= valSecond, 1_u, 0_u ) )
								{
									gsKeys[indexFirst] = valSecond;
									gsKeys[indexSecond] = valFirst;
								}
								FI

								shader::groupMemoryBarrierWithGroupSync( writer );
							}
							ROF
						}
						ROF
					}
					ROF

					// Now commit the results to global memory.
					FOR( writer, sdw::UInt, batch, 0_u, batch < batchCount, ++batch )
					{
						auto i1 = writer.declLocale( "i1", groupIndex + batch * gBatchSize );
						auto i2 = writer.declLocale( "i2", i1 + ( gBatchSize >> 1u ) );

						IF( writer, i1 < numElements )
						{
							c3d_lightClusterIndex[offset + i1] = gsKeys[i1];
						}
						FI
						IF( writer, i2 < numElements )
						{
							c3d_lightClusterIndex[offset + i2] = gsKeys[i2];
						}
						FI
					}
					ROF
				}
				, sdw::InUInt{ writer, "groupIndex" }
				, sdw::InUInt{ writer, "offset" }
				, sdw::InUInt{ writer, "numElements" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
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
						, min( sdw::UInt{ BlockSize }, clusterLights.y() ) );

					IF( writer, lightCount > 1_u )
					{
						sortLights( in.localInvocationIndex, startOffset, lightCount );
					}
					FI
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
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return clusters.getCamera().getScene()->getLightCache().hasClusteredLights() && clusters.getConfig().enablePostAssignSort; } ) )
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
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&clusters](){ return clusters.getCamera().getScene()->getLightCache().hasClusteredLights() && clusters.getConfig().enablePostAssignSort; } ) )
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
