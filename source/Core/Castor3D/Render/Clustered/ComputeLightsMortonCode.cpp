#include "Castor3D/Render/Clustered/ComputeLightsMortonCode.hpp"

#include "Castor3D/DebugDefines.hpp"
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

	namespace cmpmrt
	{
		enum BindingPoints
		{
			eCamera,
			eLights,
			eClusters,
			eLightsAABB,
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

		static ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;

			// Inputs
			C3D_Camera( writer
				, eCamera
				, 0u );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u };
			C3D_Clusters( writer
				, eClusters
				, 0u );
			C3D_LightsAABB( writer
				, eLightsAABB
				, 0u );
			C3D_PointLightMortonCodes( writer
				, ePointLightMortonCodes
				, 0u );
			C3D_SpotLightMortonCodes( writer
				, eSpotLightMortonCodes
				, 0u );
			C3D_PointLightIndices( writer
				, ePointLightIndices
				, 0u );
			C3D_SpotLightIndices( writer
				, eSpotLightIndices
				, 0u );

			auto gsAABB = writer.declSharedVariable< shader::AABB >( "gsAABB" );
			auto gsAABBRange = writer.declSharedVariable< sdw::Vec4 >( "gsAABBRange" );

			// Produce a 3k-bit morton code from a quantized coordinate.
			auto getMortonCode = writer.implementFunction< sdw::UInt >( "getMortonCode"
				, [&]( sdw::UVec3 quantizedCoord )
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
				, [&]( sdw::ComputeIn in )
				{
					auto groupIndex = in.localInvocationIndex;

					IF( writer, groupIndex == 0_u )
					{
						gsAABB = c3d_lightsAABB[0_u];
						// Compute the recipocal of the range of the AABB.
						// This is used to normalize the light coordinates within the bounds of the AABB.
						gsAABBRange = vec4( 1.0_f ) / ( gsAABB.max() - gsAABB.min() );
					}
					FI;

					shader::groupMemoryBarrierWithGroupSync( writer );
					auto coordScale = vec4( sdw::Float{ coordinateScale } );

					auto threadIndex = in.globalInvocationID.x();

					IF( writer, threadIndex < c3d_clustersData.pointLightCount() )
					{
						auto lightOffset = writer.declLocale( "lightOffset"
							, lights.getDirectionalsEnd() + threadIndex * PointLight::LightDataComponents );
						auto point = writer.declLocale( "point"
							, lights.getPointLight( lightOffset ) );
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ) );
						// Normalize and scale the position of the light to produce the quantized coordinate.
						auto quantized = writer.declLocale( "quantized"
							, sdw::uvec4( ( vsPosition - gsAABB.min() ) * gsAABBRange * coordScale ) );

						c3d_pointLightMortonCodes[threadIndex] = getMortonCode( quantized.xyz() );
						c3d_pointLightIndices[threadIndex] = lightOffset;
					}
					FI;

					IF( writer, threadIndex < c3d_clustersData.spotLightCount() )
					{
						auto lightOffset = writer.declLocale( "lightOffset"
							, lights.getPointsEnd() + threadIndex * SpotLight::LightDataComponents );
						auto spot = writer.declLocale( "spot"
							, lights.getSpotLight( lightOffset ) );
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ) );
						// Normalize and scale the position of the light to produce the quantized coordinate.
						auto quantized = writer.declLocale( "quantized"
							, sdw::uvec4( ( vsPosition - gsAABB.min() ) * gsAABBRange * coordScale ) );

						c3d_spotLightMortonCodes[threadIndex] = getMortonCode( quantized.xyz() );
						c3d_spotLightIndices[threadIndex] = lightOffset;
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
				, FrustumClusters & clusters
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "ComputeLightsMortonCode", createShader() } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 2u }
					, config
						.enabled( &clusters.getConfig().sortLights )
						.getPassIndex( GetPassIndexCallback{ [this]() { return doGetPassIndex(); } } )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
				, m_clusters{ clusters }
			{
			}

		private:
			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				for ( auto & attach : m_pass.buffers )
				{
					auto buffer = attach.buffer;

					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( buffer.buffer.buffer( index )
							, buffer.range );
						context.memoryBarrier( commandBuffer
							, buffer.buffer.buffer( index )
							, buffer.range
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}

				m_index = 0u;
			}

			uint32_t doGetPassIndex()const
			{
				if ( m_clusters.getConfig().sortLights )
				{
					m_index = 1u - m_index;
				}

				return m_index;
			}

		private:
			FrustumClusters & m_clusters;
			mutable u32 m_index{};
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createComputeLightsMortonCodePass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & pass = graph.createPass( "ComputeLightsMortonCode"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				uint32_t numThreadGroups = uint32_t( std::ceil( float( MaxLightsCount ) / 1024.0f ) );
				auto result = std::make_unique< cmpmrt::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( numThreadGroups )
						.enabled( &clusters.needsClustersUpdate() ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		cameraUbo.createPassBinding( pass, cmpmrt::eCamera );
		auto & lights = clusters.getCamera().getScene()->getLightCache();
		lights.createPassBinding( pass, cmpmrt::eLights );
		clusters.getClustersUbo().createPassBinding( pass, cmpmrt::eClusters );
		createInputStoragePassBinding( pass, uint32_t( cmpmrt::eLightsAABB ), "C3D_LightsAABB", clusters.getLightsAABBBuffer(), 0u, ashes::WholeSize );

		clusters.initPointLightMortonIndicesIO();
		clusters.initSpotLightMortonIndicesIO();
		createClearableOutputStorageBinding( pass, uint32_t( cmpmrt::ePointLightMortonCodes ), "C3D_PointLightMortonCodes", clusters.getOutputPointLightMortonCodesBuffers(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( cmpmrt::eSpotLightMortonCodes ), "C3D_SpotLightMortonCodes", clusters.getOutputSpotLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( cmpmrt::ePointLightIndices ), "C3D_PointLightIndices", clusters.getOutputPointLightIndicesBuffers(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( cmpmrt::eSpotLightIndices ), "C3D_SpotLightIndices", clusters.getOutputSpotLightIndicesBuffers(), 0u, ashes::WholeSize );
		clusters.swapPointLightMortonIndicesIO();
		clusters.swapSpotLightMortonIndicesIO();

		return pass;
	}

	//*********************************************************************************************
}
