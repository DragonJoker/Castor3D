#include "FFTOceanRendering/GenerateDisplacementPass.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace
	{
		ashes::DescriptorSetLayoutPtr createDescriptorLayout( castor3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ castor3d::makeDescriptorSetLayoutBinding( GenerateDisplacementPass::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( GenerateDisplacementPass::eInput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( GenerateDisplacementPass::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( GenerateDisplacementPass::Name 
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = pass.buffers[GenerateDisplacementPass::eConfig].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[GenerateDisplacementPass::eInput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[GenerateDisplacementPass::eOutput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			auto descriptorSet = pool.createDescriptorSet( GenerateDisplacementPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( castor3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( GenerateDisplacementPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		ashes::ComputePipelinePtr createPipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( GenerateDisplacementPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, castor3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		castor3d::ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;
			auto const G = writer.declConstant( "G", 9.81_f );

			UBO_OCEAN( writer, GenerateDisplacementPass::eConfig, 0u );

			auto distr = writer.declStorageBuffer( "Distribution", GenerateDisplacementPass::eInput, 0u );
			auto distribution = distr.declMemberArray< sdw::Vec2 >( "distribution" );
			distr.end();

			auto fft = writer.declStorageBuffer( "DisplacementFFT", GenerateDisplacementPass::eOutput, 0u );
			auto grads = fft.declMemberArray< sdw::Vec2 >( "grads" );
			fft.end();

			auto alias = writer.implementFunction< sdw::Vec2 >( "alias"
				, [&]( sdw::Vec2 i
					, sdw::Vec2 N )
				{
					writer.returnStmt( mix( i, i - N, vec2( greaterThan( i, 0.5 * N ) ) ) );
				}
				, sdw::InVec2{ writer, "i" }
				, sdw::InVec2{ writer, "n" } );

			auto cmul = writer.implementFunction< sdw::Vec2 >( "cmul"
				, [&]( sdw::Vec2 a, sdw::Vec2 b )
				{
					auto r3 = writer.declLocale( "r3", a.yx() );
					auto r1 = writer.declLocale( "r1", b.xx() );
					auto R0 = writer.declLocale( "R0", a * r1 );
					auto r2 = writer.declLocale( "r2", b.yy() );
					auto R1 = writer.declLocale( "R1", r2 * r3 );
					writer.returnStmt( R0 + vec2( -R1.x(), R1.y() ) );
				}
				, sdw::InVec2{ writer, "a" }
				, sdw::InVec2{ writer, "b" } );

			auto mix2U = writer.implementFunction< sdw::UVec2 >( "mix2U"
				, [&]( sdw::UVec2 a
					, sdw::UVec2 b
					, sdw::BVec2 v )
				{
					writer.returnStmt( uvec2( writer.ternary( v.x(), b.x(), a.x() )
						, writer.ternary( v.y(), b.y(), a.y() ) ) );
				}
				, sdw::InUVec2{ writer, "a" }
				, sdw::InUVec2{ writer, "b" }
				, sdw::InBVec2{ writer, "v" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 64u, 1u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto N = writer.declLocale( "N"
						, in.workGroupSize.xy() * in.numWorkGroups.xy() );
					auto i = writer.declLocale( "i"
						, in.globalInvocationID.xy() );
					// Pick out the negative frequency variant.
					auto wi = writer.declLocale( "wi"
						, mix2U( N - i
							, uvec2( 0_u )
							, equal( i, uvec2( 0_u ) ) ) );
					// Pick out positive and negative travelling waves.
					auto a = writer.declLocale( "a"
						, distribution[i.y() * N.x() + i.x()] );
					auto b = writer.declLocale( "b"
						, distribution[wi.y() * N.x() + wi.x()] );
					auto k = writer.declLocale( "k"
						, c3d_oceanData.otherMod * alias( vec2( i ), vec2( N ) ) );
					auto k_len = writer.declLocale( "k_len"
						, length( k ) );
					// If this sample runs for hours on end, the cosines of very large numbers will eventually become unstable.
					// It is fairly easy to fix this by wrapping uTime,
					// and quantizing w such that wrapping uTime does not change the result.
					// See Tessendorf's paper for how to do it.
					// The sqrt(G * k_len) factor represents how fast ocean waves at different frequencies propagate.
					auto w = writer.declLocale( "w"
						, sqrt( G * k_len ) * c3d_oceanData.time );
					auto cw = writer.declLocale( "cw"
						, cos( w ) );
					auto sw = writer.declLocale( "sw"
						, sin( w ) );
					// Complex multiply to rotate our frequency samples.
					a = cmul( a, vec2( cw, sw ) );
					b = cmul( b, vec2( cw, sw ) );
					b = vec2( b.x(), -b.y()  ); // Complex conjugate since we picked a frequency with the opposite direction.
					auto res = writer.declLocale( "res"
						, a + b ); // Sum up forward and backwards travelling waves.
					auto grad = writer.declLocale( "grad"
						, cmul( res, vec2( k.y() / ( k_len + 0.00001_f ), k.x() / ( k_len + 0.00001_f ) ) ) );
						//, cmul( res, vec2( -k.y() / ( k_len + 0.00001_f ), k.x() / ( k_len + 0.00001_f ) ) ) );
					grads[i.y() * N.x() + i.x()] = grad;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	castor::String const GenerateDisplacementPass::Name{ "GenerateHeightmap" };

	GenerateDisplacementPass::GenerateDisplacementPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkExtent2D const & extent
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
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, Name, createShader() }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
		, m_extent{ extent }
	{
	}

	void GenerateDisplacementPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void GenerateDisplacementPass::doInitialise()
	{
	}

	void GenerateDisplacementPass::doRecordInto( crg::RecordContext & context
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

	VkPipelineStageFlags GenerateDisplacementPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	uint32_t GenerateDisplacementPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool GenerateDisplacementPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************
}
