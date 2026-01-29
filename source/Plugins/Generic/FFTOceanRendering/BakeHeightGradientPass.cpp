#include "FFTOceanRendering/BakeHeightGradientPass.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
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

	namespace bakehg
	{
		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( c3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ c3d::makeDescriptorSetLayoutBindingT( BakeHeightGradientPass::Bindings::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBindingT( BakeHeightGradientPass::Bindings::eHeight
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBindingT( BakeHeightGradientPass::Bindings::eDisplacement
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBindingT( BakeHeightGradientPass::Bindings::eHeightDisplacement
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBindingT( BakeHeightGradientPass::Bindings::eGradientJacobian
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( BakeHeightGradientPass::Name
				, c3d::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = graph.getDescriptorWriteT( *pass.getUniforms().begin()->second
				, BakeHeightGradientPass::Bindings::eConfig );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWriteT( *pass.getInputs().find( uint32_t( BakeHeightGradientPass::Bindings::eHeight ) )->second
				, BakeHeightGradientPass::Bindings::eHeight );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWriteT( *pass.getInputs().find( uint32_t( BakeHeightGradientPass::Bindings::eDisplacement ) )->second
				, BakeHeightGradientPass::Bindings::eDisplacement );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			auto out1 = pass.getOutputs().begin();
			writes.push_back( ashes::WriteDescriptorSet{ out1->first
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( out1->second->view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );

			auto out2 = pass.getOutputs().rbegin();
			writes.push_back( ashes::WriteDescriptorSet{ out2->first
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( out2->second->view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );

			auto descriptorSet = pool.createDescriptorSet( BakeHeightGradientPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( c3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( BakeHeightGradientPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) }
				, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_COMPUTE_BIT, 0u, uint32_t( sizeof( BakeHeightGradientPass::Data ) ) } } );
		}

		static ashes::ComputePipelinePtr createPipeline( c3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, c3d::ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( BakeHeightGradientPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, c3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		static c3d::ShaderPtr createShader( c3d::RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto pcb = writer.declPushConstantsBuffer( "BakeData" );
			auto invSize = pcb.declMember< sdw::Vec4 >( "invSize" );
			auto scale = pcb.declMember< sdw::Vec4 >( "scale" );
			pcb.end();

			C3D_FftOcean( writer, BakeHeightGradientPass::Bindings::eConfig, 0u );

			auto hgt = writer.declStorageBuffer( "Height", uint32_t( BakeHeightGradientPass::Bindings::eHeight ), 0u );
			auto heights = hgt.declMemberArray< sdw::Vec2 >( "heights" );
			hgt.end();

			auto disp = writer.declStorageBuffer( "Displacement", uint32_t( BakeHeightGradientPass::Bindings::eDisplacement ), 0u );
			auto displacements = disp.declMemberArray< sdw::Vec2 >( "displacements" );
			disp.end();

			auto heightDisplacement = writer.declStorageImg< sdw::WImage2DRgba16 >( "heightDisplacement", uint32_t( BakeHeightGradientPass::Bindings::eHeightDisplacement ), 0u );
			auto gradJacobian = writer.declStorageImg< sdw::WImage2DRgba16 >( "gradJacobian", uint32_t( BakeHeightGradientPass::Bindings::eGradientJacobian ), 0u );

			auto normalizeCoord = writer.implementFunction< sdw::Int >( "normalizeCoord"
				, [&writer]( sdw::Int const & c, sdw::Int const & s )
				{
					writer.returnStmt( writer.ternary( c < 0_i
						, c + s
						, c % s ) );
				}
				, sdw::InInt{ writer, "c" }
				, sdw::InInt{ writer, "s" } );

			auto getHgtIndex = writer.implementFunction< sdw::Int >( "getHgtIndex"
				, [&writer, &c3d_oceanData, &normalizeCoord]( sdw::IVec2 const & i )
				{
					writer.returnStmt( normalizeCoord( i.y(), writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().y() ) )
						* writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().x() )
						+ normalizeCoord( i.x(), writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().x() ) ) );
				}
				, sdw::InIVec2{ writer, "i" } );

			auto getDspIndex = writer.implementFunction< sdw::Int >( "getDspIndex"
				, [&writer, &normalizeCoord, &c3d_oceanData]( sdw::IVec2 const & i )
				{
					writer.returnStmt( ( normalizeCoord( i.y(), writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().y() ) ) >> c3d_oceanData.displacementDownsample() )
						* ( writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().x() ) >> c3d_oceanData.displacementDownsample() )
						+ ( normalizeCoord( i.x(), writer.cast< sdw::Int >( c3d_oceanData.heightMapSamples().x() ) ) >> c3d_oceanData.displacementDownsample() ) );
				}
				, sdw::InIVec2{ writer, "i" } );

			auto jacobian = writer.implementFunction< sdw::Float >( "jacobian"
				, [&writer]( sdw::Vec2 const & dDdx
					, sdw::Vec2 const & dDdy )
				{
					writer.returnStmt( ( 1.0_f + dDdx.x() ) * ( 1.0_f + dDdx.y() )
						- ( dDdx.y() * dDdy.x() ) );
				}
				, sdw::InVec2{ writer, "dDdx" }
				, sdw::InVec2{ writer, "dDdy" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 8u, 8u, 1u }
				, [&writer, &getHgtIndex, &getDspIndex, &jacobian, &heights, &scale, &displacements
					, &heightDisplacement, &gradJacobian]( sdw::ComputeIn const & in )
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
			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	c3d::MbString const BakeHeightGradientPass::Name{ "BakeHeightGradient" };

	BakeHeightGradientPass::BakeHeightGradientPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, c3d::RenderDevice const & device
		, c3d::Extent2D const & extent
		, c3d::Point2f const & heightMapSize
		, uint32_t displacementDownsample
		, crg::RunnablePass::IsEnabledCallback const & isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( c3d::PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext const & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_descriptorSetLayout{ bakehg::createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ bakehg::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, c3d::makeString( Name ), bakehg::createShader( device ) }
		, m_pipeline{ bakehg::createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ bakehg::createDescriptorSet( getGraph(), *m_descriptorSetPool, getPass() ) }
		, m_extent{ extent }
		, m_heightMapSize{ heightMapSize }
		, m_displacementDownsample{ displacementDownsample }
	{
	}

	void BakeHeightGradientPass::accept( c3d::RenderTechniqueVisitor & visitor )const
	{
		visitor.visit( m_shader );
	}

	void BakeHeightGradientPass::doRecordInto( crg::RecordContext const & context
		, VkCommandBuffer commandBuffer
		, [[maybe_unused]] uint32_t index )
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
		context->vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline );
		context->vkCmdPushConstants( commandBuffer
			, *m_pipelineLayout
			, VK_SHADER_STAGE_COMPUTE_BIT
			, 0u
			, uint32_t( sizeof( BakeHeightGradientPass::Data ) )
			, &m_data );
		context->vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		context->vkCmdDispatch( commandBuffer
			, m_extent.width / 8u
			, m_extent.height / 8u
			, 1u );
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

	void createBakeHeightGradientPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Extent2D const & extent
		, c3d::Point2f const & heightMapSize
		, uint32_t displacementDownsample
		, OceanUbo const & ubo
		, c3d::Buffer const & height
		, c3d::Buffer const & displacement
		, c3d::Array< c3d::Texture, 2u > & heightDisp
		, c3d::Array< c3d::Texture, 2u > & gradJacob )
	{
		auto & pass = graph.createPass( "BakeHeightGradient"
			, [&device, extent, heightMapSize, displacementDownsample]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = c3d::makeRawUnique< BakeHeightGradientPass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, heightMapSize
					, displacementDownsample
					, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		ubo.createPassBinding( pass
			, BakeHeightGradientPass::Bindings::eConfig );
		pass.addInputStorageT( *height.getLastAttach()
			, BakeHeightGradientPass::Bindings::eHeight );
		pass.addInputStorageT( *displacement.getLastAttach()
			, BakeHeightGradientPass::Bindings::eDisplacement );
		heightDisp.front().setLastAttach( pass.addOutputStorageImageT( heightDisp.front().getTargetViewId()
			, BakeHeightGradientPass::Bindings::eHeightDisplacement ) );
		gradJacob.front().setLastAttach( pass.addOutputStorageImageT( gradJacob.front().getTargetViewId()
			, BakeHeightGradientPass::Bindings::eGradientJacobian ) );
	}

	//************************************************************************************************
}
