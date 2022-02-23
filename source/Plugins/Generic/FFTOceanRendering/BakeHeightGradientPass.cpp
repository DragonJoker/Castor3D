#include "FFTOceanRendering/BakeHeightGradientPass.hpp"

#include "FFTOceanRendering/OceanFFTRenderPass.hpp"
#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
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
			ashes::VkDescriptorSetLayoutBindingArray bindings{ castor3d::makeDescriptorSetLayoutBinding( BakeHeightGradientPass::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( BakeHeightGradientPass::eHeight
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( BakeHeightGradientPass::eDisplacement
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( BakeHeightGradientPass::eHeightDisplacement
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( BakeHeightGradientPass::eGradientJacobian
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( BakeHeightGradientPass::Name 
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = pass.buffers[BakeHeightGradientPass::eConfig].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[BakeHeightGradientPass::eHeight].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[BakeHeightGradientPass::eDisplacement].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			auto out1 = pass.images.front();
			writes.push_back( ashes::WriteDescriptorSet{ out1.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( out1.view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );

			auto out2 = pass.images.back();
			writes.push_back( ashes::WriteDescriptorSet{ out2.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( out2.view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );

			auto descriptorSet = pool.createDescriptorSet( BakeHeightGradientPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( castor3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( BakeHeightGradientPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) }
				, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_COMPUTE_BIT, 0u, uint32_t( sizeof( BakeHeightGradientPass::Data ) ) } } );
		}

		ashes::ComputePipelinePtr createPipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( BakeHeightGradientPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, castor3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		castor3d::ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;

			auto pcb = writer.declPushConstantsBuffer( "BakeData" );
			auto invSize = pcb.declMember< sdw::Vec4 >( "invSize" );
			auto scale = pcb.declMember< sdw::Vec4 >( "scale" );
			pcb.end();

			C3D_FftOcean( writer, BakeHeightGradientPass::eConfig, 0u );

			auto hgt = writer.declStorageBuffer( "Height", BakeHeightGradientPass::eHeight, 0u );
			auto heights = hgt.declMemberArray< sdw::Vec2 >( "heights" );
			hgt.end();

			auto disp = writer.declStorageBuffer( "Displacement", BakeHeightGradientPass::eDisplacement, 0u );
			auto displacements = disp.declMemberArray< sdw::Vec2 >( "displacements" );
			disp.end();

			auto heightDisplacement = writer.declStorageImg< sdw::WImage2DRgba16 >( "heightDisplacement", BakeHeightGradientPass::eHeightDisplacement, 0u );
			auto gradJacobian = writer.declStorageImg< sdw::WImage2DRgba16 >( "gradJacobian", BakeHeightGradientPass::eGradientJacobian, 0u );

			auto normalizeCoord = writer.implementFunction< sdw::Int >( "normalizeCoord"
				, [&]( sdw::Int c, sdw::Int s )
				{
					writer.returnStmt( writer.ternary( c < 0_i
						, c + s
						, c % s ) );
				}
				, sdw::InInt{ writer, "c" }
				, sdw::InInt{ writer, "s" } );

			auto getHgtIndex = writer.implementFunction< sdw::Int >( "getHgtIndex"
				, [&]( sdw::IVec2 i )
				{
					writer.returnStmt( normalizeCoord( i.y(), writer.cast< sdw::Int >( c3d_oceanData.size.y() ) )
						* writer.cast< sdw::Int >( c3d_oceanData.size.x() )
						+ normalizeCoord( i.x(), writer.cast< sdw::Int >( c3d_oceanData.size.x() ) ) );
				}
				, sdw::InIVec2{ writer, "i" } );

			auto getDspIndex = writer.implementFunction< sdw::Int >( "getDspIndex"
				, [&]( sdw::IVec2 i )
				{
					writer.returnStmt( ( normalizeCoord( i.y(), writer.cast< sdw::Int >( c3d_oceanData.size.y() ) ) >> c3d_oceanData.displacementDownsample )
						* ( writer.cast< sdw::Int >( c3d_oceanData.size.x() ) >> c3d_oceanData.displacementDownsample )
						+ ( normalizeCoord( i.x(), writer.cast< sdw::Int >( c3d_oceanData.size.x() ) ) >> c3d_oceanData.displacementDownsample ) );
				}
				, sdw::InIVec2{ writer, "i" } );

			auto jacobian = writer.implementFunction< sdw::Float >( "jacobian"
				, [&]( sdw::Vec2 const & dDdx
					, sdw::Vec2 const & dDdy )
				{
					writer.returnStmt( ( 1.0_f + dDdx.x() ) * ( 1.0_f + dDdx.y() )
						- ( dDdx.y() * dDdy.x() ) );
				}
				, sdw::InVec2{ writer, "dDdx" }
				, sdw::InVec2{ writer, "dDdy" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 8u, 8u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto i = writer.declLocale( "i"
						, ivec2( in.globalInvocationID.xy() ) );

					auto h = writer.declLocale( "h"
						, heights[getHgtIndex( i )].x() );

					// Compute the heightmap gradient by simple differentiation.
					auto x0 = writer.declLocale( "x0"
						, heights[getHgtIndex( i + ivec2( -1_i, 0_i ) )].x() );
					auto x1 = writer.declLocale( "x1"
						, heights[getHgtIndex( i + ivec2( +1_i, 0_i ) )].x() );
					auto y0 = writer.declLocale( "y0"
						, heights[getHgtIndex( i + ivec2( 0_i, -1_i ) )].x() );
					auto y1 = writer.declLocale( "y1"
						, heights[getHgtIndex( i + ivec2( 0_i, +1_i ) )].x() );
					auto grad = writer.declLocale( "grad"
						, scale.xy() * vec2( 0.5_f ) * vec2( x1 - x0, y1 - y0 ) );

					auto lambda = writer.declConstant( "lambda", vec2( 1.2_f ) );
					auto halfPixel = writer.declConstant( "halfPixel", vec2( 0.5_f ) );

					// Displacement map must be sampled with a different offset since it's a smaller texture.
					auto displacement = writer.declLocale( "displacement"
						, lambda * displacements[getDspIndex( i )] );

					// Compute jacobian.
					auto dDdx = writer.declLocale( "dDdx"
						, halfPixel * lambda
							* ( displacements[getDspIndex( i + ivec2( +1_i, 0_i ) )]
								- displacements[getDspIndex( i + ivec2( -1_i, 0_i ) )] ) );
					auto dDdy = writer.declLocale( "dDdy"
						, halfPixel * lambda
							* ( displacements[getDspIndex( i + ivec2( 0_i, +1_i ) )]
								- displacements[getDspIndex( i + ivec2( 0_i, -1_i ) )] ) );
					auto j = writer.declLocale( "j"
						, jacobian( dDdx * scale.z(), dDdy * scale.z() ) );

					// Read by vertex shader/tess shader.
					heightDisplacement.store( i, vec4( h, displacement, 0.0 ) );

					// Read by fragment shader.
					gradJacobian.store( i, vec4( grad, j, 0.0 ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	castor::String const BakeHeightGradientPass::Name{ "BakeHeightGradient" };

	BakeHeightGradientPass::BakeHeightGradientPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkExtent2D const & extent
		, castor::Point2f const & heightMapSize
		, uint32_t displacementDownsample
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
		, m_heightMapSize{ heightMapSize }
		, m_displacementDownsample{ displacementDownsample }
	{
	}

	void BakeHeightGradientPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void BakeHeightGradientPass::doInitialise()
	{
	}

	void BakeHeightGradientPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		uint32_t Nx = m_extent.width;
		uint32_t Ny = m_extent.height;
		m_data.invSize = { 1.0f / float( Nx )
			, 1.0f / float( Ny )
			, 1.0f / float( Nx >> m_displacementDownsample )
			, 1.0f / float( Ny >> m_displacementDownsample ) };
		m_data.scale = { float( Nx ) / m_heightMapSize->x
			, float( Ny ) / m_heightMapSize->y
			, float( Nx >> m_displacementDownsample ) / m_heightMapSize->x
			, float( Ny >> m_displacementDownsample ) / m_heightMapSize->y };
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline );
		m_context.vkCmdPushConstants( commandBuffer
			, *m_pipelineLayout
			, VK_SHADER_STAGE_COMPUTE_BIT
			, 0u
			, uint32_t( sizeof( BakeHeightGradientPass::Data ) )
			, &m_data );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		m_context.vkCmdDispatch( commandBuffer
			, m_extent.width / 8u
			, m_extent.height / 8u
			, 1u );
	}

	VkPipelineStageFlags BakeHeightGradientPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	uint32_t BakeHeightGradientPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool BakeHeightGradientPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************

	crg::FramePass const & createBakeHeightGradientPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, castor::Point2f const & heightMapSize
		, uint32_t displacementDownsample
		, OceanUbo const & ubo
		, ashes::BufferBase const & height
		, ashes::BufferBase const & displacement
		, std::array< castor3d::Texture, 2u > const & heightDisp
		, std::array< castor3d::Texture, 2u > const & gradJacob
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
	{
		auto & result = graph.createPass( "BakeHeightGradient"
			, [&device, extent, heightMapSize, displacementDownsample, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< BakeHeightGradientPass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, heightMapSize
					, displacementDownsample
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		ubo.createPassBinding( result
			, BakeHeightGradientPass::eConfig );
		result.addInputStorageBuffer( { height, "HeightFFTResult" }
			, BakeHeightGradientPass::eHeight
			, 0u
			, ashes::WholeSize );
		result.addInputStorageBuffer( { displacement, "DisplacementFFTResult" }
			, BakeHeightGradientPass::eDisplacement
			, 0u
			, ashes::WholeSize );
		result.addOutputStorageView( heightDisp.front().sampledViewId
			, BakeHeightGradientPass::eHeightDisplacement );
		result.addOutputStorageView( gradJacob.front().sampledViewId
			, BakeHeightGradientPass::eGradientJacobian );
		return result;
	}

	//************************************************************************************************
}
