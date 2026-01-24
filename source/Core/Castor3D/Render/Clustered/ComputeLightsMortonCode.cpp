#include "Castor3D/Render/Clustered/ComputeLightsMortonCode.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace cmpmrt
	{
		enum class Bindings
		{
			eClusters,
			eAllLightsAABB,
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
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_AllLightsAABB( writer
				, Bindings::eAllLightsAABB
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
				, [&writer, &c3d_allLightsAABB, &c3d_reducedLightsAABB, &c3d_lightsAABBRange, &c3d_clustersData
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
							, c3d_allLightsAABB[threadIndex] );
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
							, c3d_allLightsAABB[c3d_clustersData.pointLightCount() + threadIndex] );
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
				, crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
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

		private:
			FrustumClusters & m_clusters;
		};
	}

	//*********************************************************************************************

	ClustersLightSortAttachs createComputeLightsMortonCodePass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, BufferBase const & allLightsAABB
		, BufferBase const & reducedLightsAABB
		, crg::BufferViewIdArray const & pointLightMortonCodes
		, crg::BufferViewIdArray const & spotLightMortonCodes
		, crg::BufferViewIdArray const & pointLightIndices
		, crg::BufferViewIdArray const & spotLightIndices )
	{
		auto & pass = graph.createPass( "ComputeLightsMortonCode"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				constexpr uint32_t numThreadGroups = divRoundUp( MaxLightsCount, 1024u );
				auto result = makeRawUnique< cmpmrt::FramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( numThreadGroups ) );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clusters.getClustersUbo().createPassBinding( pass, cmpmrt::Bindings::eClusters );
		pass.addInputStorageT( *allLightsAABB.getLastAttach(), cmpmrt::Bindings::eAllLightsAABB );
		pass.addInputStorageT( *reducedLightsAABB.getLastAttach(), cmpmrt::Bindings::eReducedLightsAABB );
		return { pass.addClearableOutputStorageBufferT( pointLightMortonCodes, cmpmrt::Bindings::ePointLightMortonCodes )
			, pass.addClearableOutputStorageBufferT( spotLightMortonCodes, cmpmrt::Bindings::eSpotLightMortonCodes )
			, pass.addClearableOutputStorageBufferT( pointLightIndices, cmpmrt::Bindings::ePointLightIndices )
			, pass.addClearableOutputStorageBufferT( spotLightIndices, cmpmrt::Bindings::eSpotLightIndices ) };
	}

	//*********************************************************************************************
}
