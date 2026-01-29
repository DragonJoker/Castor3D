#include "Castor3D/Render/Clustered/BucketSortLights.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslRadixSort.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace buksrt
	{
		enum class Bindings
		{
			eInputKeys,
			eInputValues,
			eOutputKeys,
			eOutputValues,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto inputKeysBuffer = writer.declStorageBuffer( "c3d_inputKeysBuffer"
				, Bindings::eInputKeys
				, 0u );
			auto c3d_inputKeys = inputKeysBuffer.declMemberArray< sdw::UInt >( "ik" );
			inputKeysBuffer.end();

			auto inputValuesBuffer = writer.declStorageBuffer( "c3d_inputValuesBuffer"
				, Bindings::eInputValues
				, 0u );
			auto c3d_inputValues = inputValuesBuffer.declMemberArray< sdw::UInt >( "iv" );
			inputValuesBuffer.end();

			auto outputKeysBuffer = writer.declStorageBuffer( "c3d_outputKeysBuffer"
				, Bindings::eOutputKeys
				, 0u );
			auto c3d_outputKeys = outputKeysBuffer.declMemberArray< sdw::UInt >( "ok" );
			outputKeysBuffer.end();

			auto outputValuesBuffer = writer.declStorageBuffer( "c3d_outputValuesBuffer"
				, Bindings::eOutputValues
				, 0u );
			auto c3d_outputValues = outputValuesBuffer.declMemberArray< sdw::UInt >( "ov" );
			outputValuesBuffer.end();

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
			pcb.end();

			shader::RadixSortT< 4u > bucket{ writer, 30u };

			writer.implementMainT< sdw::VoidT >( bucket.threadsCount
				, [&writer, &bucket, &c3d_numElements, &c3d_inputKeys, &c3d_inputValues, &c3d_outputKeys, &c3d_outputValues]( sdw::ComputeIn const & in )
				{
					//// In our case, the input keys are 30-bit morton codes.
					bucket.sortT( writer
						, 0_u, c3d_numElements
						, in.localInvocationIndex, in.globalInvocationID.x()
						, c3d_inputKeys, c3d_outputKeys
						, c3d_inputValues, c3d_outputValues, 0xFFFFFFFF_u );
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
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						, [this]( crg::RecordContext const & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
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

			void resetPipeline( uint32_t index )
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
					, FramePass const * parent
					, LightType lightType )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "BucketSort/" ) + getName( lightType ), createShader( device ) }
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
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 1u }
				{
				}
			};

		private:
			struct DispatchData
			{
				u32 numElements;
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
				return m_clusters.needsLightsUpdate()
					&& m_lightCache.getLightsBufferCount( m_lightType ) > 0;
			}

			void doRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// Build bottom level of the BVH.
				auto lightsCount = m_lightCache.getLightsBufferCount( m_lightType );
				auto numThreadGroups = divRoundUp( lightsCount, FrustumClusters::getBucketSortBucketSize() );
				DispatchData data{ lightsCount };
				m_pipeline.pipeline.recordInto( context, commandBuffer, index );
				context->vkCmdPushConstants( commandBuffer, m_pipeline.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u, &data );
				context->vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
			}

			void doCreatePipeline( uint32_t index
				, Pipeline & pipeline )const
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

	ClustersLightSortAttachs createBucketSortLightsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, ClustersLightSortAttachs const & sortAttachs
		, crg::BufferViewIdArray const & pointLightMortonCodes
		, crg::BufferViewIdArray const & spotLightMortonCodes
		, crg::BufferViewIdArray const & pointLightIndices
		, crg::BufferViewIdArray const & spotLightIndices )
	{
		ClustersLightSortAttachs result{};
		{
			// Point lights
			auto & point = graph.createPass( "BucketSort/Point"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto runPass = makeRawUnique< buksrt::FramePass >( framePass
						, context
						, runnableGraph
						, device
						, clusters
						, LightType::ePoint );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			point.addInputStorageT( *sortAttachs.pointLightMortonCodes, buksrt::Bindings::eInputKeys );
			point.addInputStorageT( *sortAttachs.pointLightIndices, buksrt::Bindings::eInputValues );
			result.pointLightMortonCodes = point.addClearableOutputStorageBufferT( pointLightMortonCodes, buksrt::Bindings::eOutputKeys );
			result.pointLightIndices = point.addClearableOutputStorageBufferT( pointLightIndices, buksrt::Bindings::eOutputValues );
		}
		{
			// Spot lights
			auto & spot = graph.createPass( "BucketSort/Spot"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runnableGraph )
				{
					auto runPass = makeRawUnique< buksrt::FramePass >( framePass
						, context
						, runnableGraph
						, device
						, clusters
						, LightType::eSpot );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			spot.addInputStorageT( *sortAttachs.spotLightMortonCodes, buksrt::Bindings::eInputKeys );
			spot.addInputStorageT( *sortAttachs.spotLightIndices, buksrt::Bindings::eInputValues );
			result.spotLightMortonCodes = spot.addClearableOutputStorageBufferT( spotLightMortonCodes, buksrt::Bindings::eOutputKeys );
			result.spotLightIndices = spot.addClearableOutputStorageBufferT( spotLightIndices, buksrt::Bindings::eOutputValues );
		}
		return result;
	}

	//*********************************************************************************************
}
