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
#include "Castor3D/Shader/Shaders/GlslRadixSort.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace radix
	{
		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };
			// In our case, the input keys are 30-bit morton codes.
			shader::radix::createShaderT< 30u, 4u >( writer, sdw::UInt{ UINT_MAX } );
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
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "RadixSort/" ) + getName( lightType ), createShader( device ) }
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
				auto numThreadGroups = castor::divRoundUp( lightsCount, shader::radix::sortBucketSizeT< 4u > );
				DispatchData data{ lightsCount };
				m_pipeline.pipeline.recordInto( context, commandBuffer, index );
				m_context.vkCmdPushConstants( commandBuffer, m_pipeline.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u, &data );
				m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
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

	crg::FramePassArray createRadixSortLightsPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, FrustumClusters & clusters )
	{
		// Point lights
		auto & point = graph.createPass( "RadixSort/Point"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = castor::make_unique< radix::FramePass >( framePass
					, context
					, runnableGraph
					, device
					, clusters
					, LightType::ePoint );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		point.addDependency( *previousPass );
		createInputStoragePassBinding( point, uint32_t( shader::radix::eInputKeys ), cuT( "C3D_InLightMortonCodes" ), clusters.getOutputPointLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( point, uint32_t( shader::radix::eInputValues ), cuT( "C3D_InLightIndices" ), clusters.getOutputPointLightIndicesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( shader::radix::eOutputKeys ), cuT( "C3D_OutLightMortonCodes" ), clusters.getInputPointLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( shader::radix::eOutputValues ), cuT( "C3D_OutLightIndices" ), clusters.getInputPointLightIndicesBuffer(), 0u, ashes::WholeSize );

		// Spot lights
		auto & spot = graph.createPass( "RadixSort/Spot"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto result = castor::make_unique< radix::FramePass >( framePass
					, context
					, runnableGraph
					, device
					, clusters
					, LightType::eSpot );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		spot.addDependency( *previousPass );
		createInputStoragePassBinding( spot, uint32_t( shader::radix::eInputKeys ), cuT( "C3D_InLightMortonCodes" ), clusters.getOutputSpotLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( spot, uint32_t( shader::radix::eInputValues ), cuT( "C3D_InLightIndices" ), clusters.getOutputSpotLightIndicesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( shader::radix::eOutputKeys ), cuT( "C3D_OutLightMortonCodes" ), clusters.getInputSpotLightMortonCodesBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( shader::radix::eOutputValues ), cuT( "C3D_OutLightIndices" ), clusters.getInputSpotLightIndicesBuffer(), 0u, ashes::WholeSize );

		return { &point, &spot };
	}

	//*********************************************************************************************
}
