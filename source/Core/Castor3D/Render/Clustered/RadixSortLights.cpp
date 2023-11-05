#include "Castor3D/Render/Clustered/RadixSortLights.hpp"

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

	namespace radix
	{
		static uint32_t constexpr NumThreads = 256u;
		static uint32_t constexpr NumThreadsPerThreadGroup = 256u;

		enum BindingPoints
		{
			eInputKeys,
			eInputValues,
			eOutputKeys,
			eOutputValues,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto inputKeysBuffer = writer.declStorageBuffer( "c3d_inputKeysBuffer"
				, uint32_t( eInputKeys )
				, 0u );
			auto c3d_inputKeys = inputKeysBuffer.declMemberArray< sdw::UInt >( "ik" );
			inputKeysBuffer.end();

			auto inputValuesBuffer = writer.declStorageBuffer( "c3d_inputValuesBuffer"
				, uint32_t( eInputValues )
				, 0u );
			auto c3d_inputValues = inputValuesBuffer.declMemberArray< sdw::UInt >( "iv" );
			inputValuesBuffer.end();

			auto outputKeysBuffer = writer.declStorageBuffer( "c3d_outputKeysBuffer"
				, uint32_t( eOutputKeys )
				, 0u );
			auto c3d_outputKeys = outputKeysBuffer.declMemberArray< sdw::UInt >( "ok" );
			outputKeysBuffer.end();

			auto outputValuesBuffer = writer.declStorageBuffer( "c3d_outputValuesBuffer"
				, uint32_t( eOutputValues )
				, 0u );
			auto c3d_outputValues = outputValuesBuffer.declMemberArray< sdw::UInt >( "ov" );
			outputValuesBuffer.end();

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
			auto c3d_chunkSize = pcb.declMember< sdw::UInt >( "c3d_chunkSize" );
			pcb.end();

			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", NumThreads );		// A temporary buffer to store the input keys.                                          (1,024 Bytes)
			auto gsValues = writer.declSharedVariable< sdw::UInt >( "gsValues", NumThreads );	// A temporary buffer to store the input values.                                        (1,024 Bytes)
			auto gsE = writer.declSharedVariable< sdw::UInt >( "gsE", NumThreads );				// Set a 1 for all false sort keys (b == 0) and a 0 for all true sort keys (b == 1)     (1,024 Bytes)
			auto gsF = writer.declSharedVariable< sdw::UInt >( "gsF", NumThreads );				// Scan the splits. This results in the output index of all false sort keys (b == 0)    (1,024 Bytes)
			auto gsD = writer.declSharedVariable< sdw::UInt >( "gsD", NumThreads );				// The desination index for the ouput key and value.                                    (1,024 Bytes)
			auto gsTotalFalses = writer.declSharedVariable< sdw::UInt >( "gsTotalFalses" );		// The result of e[NUM_THREADS - 1] + f[NUM_THREADS - 1];                               (4 Bytes)

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn in )
				{
					// The number of bits to consider sorting.
					// In this case, the input keys are 30-bit morton codes.
					const u32 NumBits = 30u;

					auto groupIndex = in.localInvocationIndex;
					auto threadIndex = in.globalInvocationID.x();

					// Store the input key and values into shared memory.
					gsKeys[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputKeys[threadIndex], sdw::UInt{ UINT_MAX } );
					gsValues[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputValues[threadIndex], sdw::UInt{ UINT_MAX } );

					// Loop over the bits starting at the least-significant bit.
					FOR( writer, sdw::UInt, b, 0_u, b < NumBits, ++b )
					{
						// 1. In a temporary buffer in shared memory, we set a 1 for all false 
						//    sort keys (b = 0) and a 0 for all true sort keys.
						gsE[groupIndex] = writer.ternary( ( ( gsKeys[groupIndex] >> b ) & 1_u ) == 0_u
							, 1_u
							, 0_u );

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						IF( writer, groupIndex == 0_u )
						{
							gsF[groupIndex] = 0_u;
						}
						ELSE
						{
							gsF[groupIndex] = gsE[groupIndex - 1_u];
						}
						FI;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );
						auto temp = writer.declLocale( "temp", 0_u );

						// 2. We then scan (prefix sum) this buffer. This is the enumerate operation; 
						//    each false sort key now contains its destination address in the scan 
						//    output, which we will call f. These first two steps are equivalent to 
						//    a stream compaction operation on all false sort keys.
						for ( u32 i = 1; i < NumThreads; i <<= 1u )
						{
							temp = gsF[groupIndex];

							IF( writer, groupIndex > i )
							{
								temp += gsF[groupIndex - i];
							}
							FI;

							// Sync group shared memory reads before writes.
							shader::groupMemoryBarrierWithGroupSync( writer );

							gsF[groupIndex] = temp;

							// Sync group shared memory writes.
							shader::groupMemoryBarrierWithGroupSync( writer );
						}

						// 3. The last element in the scan's output now contains the total 
						//    number of false sort keys. We write this value to a shared 
						//    variable, gs_TotalFalses.
						IF ( writer, groupIndex == 0_u )
						{
							gsTotalFalses = gsE[NumThreads - 1u] + gsF[NumThreads - 1u];
						}
						FI;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// 4. Now we compute the destination address for the true sort keys. For 
						// a sort key at index i, this address is t = i - f + totalFalses. We 
						// then select between t and f depending on the value of b to get the 
						// destination address d of each fragment.
						gsD[groupIndex] = writer.ternary( gsE[groupIndex] == 1u
							, gsF[groupIndex]
							, groupIndex - gsF[groupIndex] + gsTotalFalses );

						// 5. Finally, we scatter the original sort keys to destination address 
						//    d. The scatter pattern is a perfect permutation of the input, so 
						//    we see no write conflicts with this scatter.
						auto key = writer.declLocale( "key", gsKeys[groupIndex] );
						auto value = writer.declLocale( "value", gsValues[groupIndex] );

						// Sync group shared memory reads before writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						gsKeys[gsD[groupIndex]] = key;
						gsValues[gsD[groupIndex]] = value;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );
					}
					ROF;

					// Now commit the results to global memory.
					c3d_outputKeys[threadIndex] = gsKeys[groupIndex];
					c3d_outputValues[threadIndex] = gsValues[groupIndex];
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::RunnablePass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters & clusters
				, LightType lightType )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, { [this]( uint32_t index ){ doInitialise( index ); }
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
						, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
						, GetPassIndexCallback( [](){ return 0u; } )
						, IsEnabledCallback( [this](){ return doIsEnabled(); } )
						, IsComputePassCallback( [](){ return true; } ) }
					, crg::ru::Config{ 1u, true /* resettable */ } }
				, m_clusters{ clusters }
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
				, m_lightType{ lightType }
				, m_pipeline{ framePass, context, graph, device, this, m_lightType }
			{
			}

			CRG_API void resetPipeline( crg::VkPipelineShaderStageCreateInfoArray config
				, uint32_t index )
			{
				resetCommandBuffer( index );
				m_pipeline.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_pipeline.createInfo ), index );
				doCreatePipeline( index, m_pipeline );
				reRecordCurrent();
			}

		private:
			struct Pipeline
			{
				ShaderModule shader;
				ashes::PipelineShaderStageCreateInfoArray createInfo;
				crg::cp::ConfigData cpConfig;
				crg::PipelineHolder pipeline;

				Pipeline( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph
					, RenderDevice const & device
					, FramePass * parent
					, LightType lightType )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT, "RadixSort/" + getName( lightType ), createShader( device ) }
					, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
					, cpConfig{ crg::getDefaultV< InitialiseCallback >()
						, nullptr
						, IsEnabledCallback( [parent]() { return parent->doIsEnabled(); } )
						, GetPassIndexCallback( []() { return 0u; } )
						, crg::getDefaultV< RecordCallback >()
						, crg::getDefaultV< RecordCallback >()
						, 1u
						, 1u
						, 1u }
					, pipeline{ framePass
						, context
						, graph
						, crg::pp::Config{}
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( createInfo ) )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 1u }
				{
				}
			};

		private:
			struct DispatchData
			{
				u32 numElements;
				u32 chunkSize;
			};

			FrustumClusters & m_clusters;
			LightCache const & m_lightCache;
			LightType m_lightType;
			Pipeline m_pipeline;

		private:
			void doInitialise( uint32_t index )
			{
				m_pipeline.pipeline.initialise();
				doCreatePipeline( index, m_pipeline );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().sortLights
					&& m_clusters.needsLightsUpdate()
					&& m_lightCache.getLightsBufferCount( m_lightType ) > 0;
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// Build bottom level of the BVH.
				auto lightsCount = m_lightCache.getLightsBufferCount( m_lightType );
				auto numThreadGroups = castor::divRoundUp( lightsCount, NumThreadsPerThreadGroup );
				DispatchData data{ lightsCount, 0u };
				m_pipeline.pipeline.recordInto( context, commandBuffer, index );
				m_context.vkCmdPushConstants( commandBuffer, m_pipeline.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
				m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
			}

			void doCreatePipeline( uint32_t index
				, Pipeline & pipeline )
			{
				auto & program = pipeline.pipeline.getProgram( index );
				VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
					, nullptr
					, 0u
					, program.front()
					, pipeline.pipeline.getPipelineLayout()
					, VkPipeline{}
					, 0u };
				pipeline.pipeline.createPipeline( index, createInfo );
			}
		};
	}

	//*********************************************************************************************

	crg::FramePassArray createRadixSortLightsPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, FrustumClusters & clusters )
	{
		// Point lights
		auto & point = graph.createPass( "RadixSort/Point"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< radix::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, LightType::ePoint );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		point.addDependency( *previousPass );
		createInputStoragePassBinding( point, uint32_t( radix::eInputKeys ), "C3D_InLightMortonCodes", clusters.getOutputPointLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( point, uint32_t( radix::eInputValues ), "C3D_InLightIndices", clusters.getOutputPointLightIndicesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( radix::eOutputKeys ), "C3D_OutLightMortonCodes", clusters.getInputPointLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( radix::eOutputValues ), "C3D_OutLightIndices", clusters.getInputPointLightIndicesBuffer(), 0u, ashes::WholeSize );

		// Spot lights
		auto & spot = graph.createPass( "RadixSort/Spot"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< radix::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, LightType::eSpot );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		spot.addDependency( *previousPass );
		createInputStoragePassBinding( spot, uint32_t( radix::eInputKeys ), "C3D_InLightMortonCodes", clusters.getOutputSpotLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( spot, uint32_t( radix::eInputValues ), "C3D_InLightIndices", clusters.getOutputSpotLightIndicesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( radix::eOutputKeys ), "C3D_OutLightMortonCodes", clusters.getInputSpotLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( radix::eOutputValues ), "C3D_OutLightIndices", clusters.getInputSpotLightIndicesBuffer(), 0u, ashes::WholeSize );

		return { &point, &spot };
	}

	//*********************************************************************************************
}
