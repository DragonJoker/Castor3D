#include "FFTOceanRendering/GenerateDistributionPass.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/RunnableGraph.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace
	{
		ashes::DescriptorSetLayoutPtr createDescriptorLayout( castor3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ castor3d::makeDescriptorSetLayoutBinding( GenerateDistributionPass::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( GenerateDistributionPass::eInput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( GenerateDistributionPass::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( GenerateDistributionPass::Name 
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = pass.buffers[GenerateDistributionPass::eConfig].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[GenerateDistributionPass::eInput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[GenerateDistributionPass::eOutput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			auto descriptorSet = pool.createDescriptorSet( GenerateDistributionPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( castor3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( GenerateDistributionPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		ashes::ComputePipelinePtr createPipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( GenerateDistributionPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, castor3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		castor3d::ShaderPtr createShader( bool normals )
		{
			sdw::ComputeWriter writer;
			auto const G = writer.declConstant( "G", 9.81_f );

			UBO_OCEAN( writer, GenerateDistributionPass::eConfig, 0u );

			auto seeds = writer.declStorageBuffer( "Seed", GenerateDistributionPass::eInput, 0u );
			auto seed = seeds.declMemberArray< sdw::Vec2 >( "seed" );
			seeds.end();

			auto distribs = writer.declStorageBuffer( "Distribution", GenerateDistributionPass::eOutput, 0u );
			auto distribution = distribs.declMemberArray< sdw::Vec2 >( "distribution" );
			distribs.end();

			auto alias = writer.implementFunction< sdw::Vec2 >( "alias"
				, [&]( sdw::Vec2 i
					, sdw::Vec2 N )
				{
					writer.returnStmt( mix( i, i - N, vec2( greaterThan( i, 0.5 * N ) ) ) );
				}
				, sdw::InVec2{ writer, "i" }
				, sdw::InVec2{ writer, "n" } );

			auto phillips = writer.implementFunction< sdw::Vec2 >( "phillips"
				, [&]( sdw::Vec2 k
					, sdw::Float maxWaveLength
					, sdw::Vec2 windDirection
					, sdw::Float L )
				{
					auto kLen = writer.declLocale( "kLen"
						, length( k ) );

					IF( writer, kLen == 0.0_f )
					{
						writer.returnStmt( vec2( 0.0_f ) );
					}
					FI;

					auto kL = writer.declLocale( "kL"
						, kLen * L );
					auto kDir = writer.declLocale( "kDir"
						, normalize( k ) );
					auto kw = writer.declLocale( "kw"
						, dot( kDir, windDirection ) );
					writer.returnStmt( vec2( pow( kw * kw, 1.0_f )							// Directional
							* exp( -1.0_f * kLen * kLen * maxWaveLength * maxWaveLength )	// Suppress small waves at ~maxWaveLength.
							* exp( -1.0_f / ( kL * kL ) )
							* pow( kLen, -4.0_f )
						, 0.0_f ) );
				}
				, sdw::InVec2{ writer, "k" }
				, sdw::InFloat{ writer, "maxWaveLength" }
				, sdw::InVec2{ writer, "windDirection" }
				, sdw::InFloat{ writer, "L" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 64u, 1u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto N = writer.declLocale( "N"
						, in.workGroupSize.xy() * in.numWorkGroups.xy() );
					auto i = writer.declLocale( "i"
						, in.globalInvocationID.xy() );

					if ( normals )
					{
						auto k = writer.declLocale( "k"
							, c3d_oceanData.normalMod * alias( vec2( i ), vec2( N ) ) );
						distribution[i.y() * N.x() + i.x()] = seed[i.y() * N.x() + i.x()]
							* vec2( c3d_oceanData.amplitude * sqrt( c3d_oceanData.normalScale.x() * c3d_oceanData.normalScale.y() ) )
							* sqrt( vec2( 0.5_f ) * phillips( k, c3d_oceanData.maxWaveLength, c3d_oceanData.windDirection, c3d_oceanData.L ) );
					}
					else
					{
						auto k = writer.declLocale( "k"
							, c3d_oceanData.otherMod * alias( vec2( i ), vec2( N ) ) );
						distribution[i.y() * N.x() + i.x()] = seed[i.y() * N.x() + i.x()]
							* vec2( c3d_oceanData.amplitude )
							* sqrt( vec2( 0.5_f ) * phillips( k, c3d_oceanData.maxWaveLength, c3d_oceanData.windDirection, c3d_oceanData.L ) );
					}
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	castor::String const GenerateDistributionPass::Name{ "GenerateDistribution" };

	GenerateDistributionPass::GenerateDistributionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkExtent2D const & extent
		, bool normals
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_descriptorSetLayout{ createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, Name, createShader( normals ) }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
		, m_extent{ extent }
	{
	}

	void GenerateDistributionPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void GenerateDistributionPass::doInitialise()
	{
	}

	void GenerateDistributionPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		m_context.vkCmdDispatch( commandBuffer
			, m_extent.width / 64u
			, m_extent.height
			, 1u );
	}

	VkPipelineStageFlags GenerateDistributionPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	uint32_t GenerateDistributionPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool GenerateDistributionPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************

	crg::FramePass const & createGenerateDistributionPass( castor::String const & prefix
		, castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, bool normals
		, OceanUbo const & ubo
		, ashes::BufferBase const & input
		, ashes::BufferBase const & output
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
	{
		auto & result = graph.createPass( "GenerateDistribution" + name
			, [&device, normals, extent, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< GenerateDistributionPass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, normals
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		ubo.createPassBinding( result
			, GenerateDistributionPass::eConfig );
		result.addInputStorageBuffer( { input, name + "Seed" }
			, GenerateDistributionPass::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { output, name + "Distribution" }
			, GenerateDistributionPass::eOutput
			, 0u
			, output.getSize() );
		return result;
	}

	//************************************************************************************************
}
