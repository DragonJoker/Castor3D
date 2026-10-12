#include "Castor3D/Render/Clustered/SortLights.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Passes/BucketMergeSort.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace morton
	{
		enum class Bindings
		{
			eAllLightsAABB,
			eClusters = eAllLightsAABB + MaxLightsAABBPageCount,
			eReducedLightsAABB,
			ePointLightMortonCodes,
			eSpotLightMortonCodes,
			ePointLightIndices,
			eSpotLightIndices,
		};

		static uint32_t constexpr NumThreads = 1024u;
		// Generate 3k-bit morton codes.
		// For k = 10, morton codes will be 30-bits.
		static u32 constexpr kBitMortonCode = 10u;
		// To quantize the light's position, the light's view space position will 
		// be normalized based on the AABB that encompases all lights.
		// The normalized value will then be scaled based on the k-bits of the morton 
		// code and the resulting bits of the x, y, and z components will be interleved
		// to produce the final morton code.
		static float constexpr coordinateScale = float( ( 1u << kBitMortonCode ) - 1u ); // This is equivalent to 2^k-1 which results in a value that when scaled by 1 will produce a number that is exactly k bits.

		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			shader::AllLightsAABB allLightsAABB( writer
				, Bindings::eAllLightsAABB
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_ReducedLightsAABB( writer
				, Bindings::eReducedLightsAABB
				, 0u );
			C3D_PointLightMortonCodes( writer
				, Bindings::ePointLightMortonCodes
				, 0u );
			C3D_SpotLightMortonCodes( writer
				, Bindings::eSpotLightMortonCodes
				, 0u );
			C3D_PointLightIndices( writer
				, Bindings::ePointLightIndices
				, 0u );
			C3D_SpotLightIndices( writer
				, Bindings::eSpotLightIndices
				, 0u );

			auto gsAABB = writer.declSharedVariable< shader::AABB >( "gsAABB" );
			auto gsAABBRange = writer.declSharedVariable< sdw::Vec3 >( "gsAABBRange" );

			// Produce a 3k-bit morton code from a quantized coordinate.
			auto getMortonCode = writer.implementFunction< sdw::UInt >( "getMortonCode"
				, [&writer]( sdw::UVec3 const & quantizedCoord )
				{
					auto mortonCode = writer.declLocale( "mortonCode", 0_u );
					auto bitMask = 1u;
					auto bitShift = 0u;
					auto kBits = 1u << kBitMortonCode;

					while ( bitMask < kBits )
					{
						// Interleave the bits of the X, Y, and Z coordinates to produce the final Morton code.
						mortonCode |= ( quantizedCoord.x() & bitMask ) << ( bitShift + 0_u );
						mortonCode |= ( quantizedCoord.y() & bitMask ) << ( bitShift + 1_u );
						mortonCode |= ( quantizedCoord.z() & bitMask ) << ( bitShift + 2_u );

						bitMask <<= 1u;
						bitShift += 2u;
					}

					writer.returnStmt( mortonCode );
				}
			, sdw::InUVec3{ writer, "quantizedCoord" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&writer, &allLightsAABB, &c3d_reducedLightsAABB, &c3d_lightsAABBRange, &c3d_clustersData
					, c3d_pointLightMortonCodes, &c3d_pointLightIndices, &c3d_spotLightMortonCodes, &c3d_spotLightIndices
					, &gsAABB, &gsAABBRange, &getMortonCode]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;

					sdwIF( writer, groupIndex == 0_u )
					{
						gsAABB = c3d_reducedLightsAABB[0_u];
						gsAABBRange = c3d_lightsAABBRange.xyz();
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );
					auto coordScale = vec3( sdw::Float{ coordinateScale } );

					auto threadIndex = in.globalInvocationID.x();

					sdwIF( writer, threadIndex < c3d_clustersData.pointLightCount() )
					{
						auto aabb = writer.declLocale( "threadIndex"
							, allLightsAABB[threadIndex] );
						auto vsPosition = writer.declLocale( "vsPosition"
							, ( aabb.min() + ( aabb.max() - aabb.min() ) / 2.0_f ).xyz() );
						// Normalize and scale the position of the light to produce the quantized coordinate.
						auto quantized = writer.declLocale( "quantized"
							, sdw::uvec3( ( vsPosition - gsAABB.min().xyz() ) * gsAABBRange * coordScale ) );

						c3d_pointLightMortonCodes[threadIndex] = getMortonCode( quantized );
						c3d_pointLightIndices[threadIndex] = threadIndex;
					}
					sdwFI

					sdwIF( writer, threadIndex < c3d_clustersData.spotLightCount() )
					{
						auto aabb = writer.declLocale( "threadIndex"
							, allLightsAABB[c3d_clustersData.pointLightCount() + threadIndex] );
						auto vsPosition = writer.declLocale( "vsPosition"
							, ( aabb.min() + ( aabb.max() - aabb.min() ) / 2.0_f ).xyz() );
						// Normalize and scale the position of the light to produce the quantized coordinate.
						auto quantized = writer.declLocale( "quantized"
							, sdw::uvec3( ( vsPosition - gsAABB.min().xyz() ) * gsAABBRange * coordScale ) );

						c3d_spotLightMortonCodes[threadIndex] = getMortonCode( quantized );
						c3d_spotLightIndices[threadIndex] = threadIndex;
					}
					sdwFI
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
				, FrustumClusters & clusters
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ComputeLightsMortonCode" ), createShader( device ) } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{ framePass
				, context
				, graph
				, crg::ru::Config{ 1u }
				, config
				.isEnabled( crg::IsEnabledCallback( [this]()
					{
						return doIsEnabled();
					} ) )
				.getGroupCountX( crg::cp::GetGroupCountCallback( [this]()
					{
						return doGetGroupsCountX();
					} ) )
				.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) ) }
				, m_clusters{ clusters }
			{
			}

		private:
			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsClustersUpdate();
			}

			u32 doGetGroupsCountX()const
			{
				auto const & lightCache = m_clusters.getScene().getLightCache();
				auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
				auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
				auto totalValues = std::max( pointLightsCount, spoLightsCount );
				return divRoundUp( totalValues, NumThreads );
			}

		private:
			FrustumClusters & m_clusters;
		};
	}

	//*********************************************************************************************

	void createComputeLightsMortonCodePass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, PagedBufferBase const & allLightsAABB
		, BufferBase const & reducedLightsAABB
		, BufferBase & pointLightMortonCodes
		, BufferBase & spotLightMortonCodes
		, BufferBase & pointLightIndices
		, BufferBase & spotLightIndices )
	{
		auto & pass = graph.createPass( "ComputeLightsMortonCode"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< morton::FramePass >( framePass, context, runGraph
					, device, clusters
					, crg::cp::Config{} );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clusters.getClustersUbo().createPassBinding( pass, morton::Bindings::eClusters );
		pass.addInputStorageT( *allLightsAABB.getLastAttach(), morton::Bindings::eAllLightsAABB );
		pass.addInputStorageT( *reducedLightsAABB.getLastAttach(), morton::Bindings::eReducedLightsAABB );
		pointLightMortonCodes.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightMortonCodes.bufferViewId, morton::Bindings::ePointLightMortonCodes ) );
		spotLightMortonCodes.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightMortonCodes.bufferViewId, morton::Bindings::eSpotLightMortonCodes ) );
		pointLightIndices.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightIndices.bufferViewId, morton::Bindings::ePointLightIndices ) );
		spotLightIndices.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightIndices.bufferViewId, morton::Bindings::eSpotLightIndices ) );
	}

	//*********************************************************************************************

	C3D_API void createSortLightsPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, FrustumClusters & clusters
		, PagedBufferBase const & allLightsAABB
		, BufferBase const & reducedLightsAABB
		, BufferBase & pointLightIndices
		, BufferBase & spotLightIndices )
	{
		BufferT< u32 > pointMortonCodes{ device, resources
			, cuT( "C3D_PointLightMortonCodes" )
			, BufferCreateFlags::eNone
			, MaxLightsCount * 2u // [0, MaxLightsCount[ => base, [MaxLightsCount, MaxLightsCount * 2[ => work
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc };
		BufferT< u32 > spotMortonCodes{ device, resources
			, cuT( "C3D_SpotLightMortonCodes" )
			, BufferCreateFlags::eNone
			, MaxLightsCount * 2u // [0, MaxLightsCount[ => base, [MaxLightsCount, MaxLightsCount * 2[ => work
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst | BufferUsageFlags::eTransferSrc };
		createComputeLightsMortonCodePass( graph, device, clusters
			, allLightsAABB, reducedLightsAABB
			, pointMortonCodes, spotMortonCodes
			, pointLightIndices, spotLightIndices );

		auto getPointLightCount = [&clusters]()
			{
				return clusters.getScene().getLightCache().getLightsBufferCount( LightType::ePoint );
			};
		auto isPointEnabled = [&clusters]()
			{
				return clusters.needsLightsUpdate()
					&& clusters.getScene().getLightCache().getLightsBufferCount( LightType::ePoint ) > 0;
			};
		auto getSpotLightCount = [&clusters]()
			{
				return clusters.getScene().getLightCache().getLightsBufferCount( LightType::eSpot );
			};
		auto isSpotEnabled = [&clusters]()
			{
				return clusters.needsLightsUpdate()
					&& clusters.getScene().getLightCache().getLightsBufferCount( LightType::eSpot ) > 0;
			};
		createBucketMergeSortPasses( graph, device, resources, cuT( "PointLights" )
			, getPointLightCount, isPointEnabled
			, pointMortonCodes, pointLightIndices );
		createBucketMergeSortPasses( graph, device, resources, cuT( "SpotLights" )
			, getSpotLightCount, isSpotEnabled
			, spotMortonCodes, spotLightIndices );
	}

	//*********************************************************************************************
}
