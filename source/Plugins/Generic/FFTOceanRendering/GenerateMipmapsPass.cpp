#include "FFTOceanRendering/GenerateMipmapsPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
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

	namespace genmips
	{
		c3d::MbString const Name{ "GenerateMipmaps" };

		enum Bindings : uint32_t
		{
			eInput,
			eOutput,
		};

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( c3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ c3d::makeDescriptorSetLayoutBinding( eInput
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBinding( eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( Name
				, c3d::move( bindings ) );
		}

		static c3d::Vector< ashes::DescriptorSetPtr > createDescriptorSets( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			c3d::Vector< ashes::DescriptorSetPtr > result;
			auto & srcDstAttach = *pass.inouts.begin()->second;
			auto inViewId = srcDstAttach.view();
			auto imageId = inViewId.data->image;
			auto data = *srcDstAttach.view().data;
			auto range = data.info.subresourceRange;
			data.info.subresourceRange.levelCount = 1u;
			auto sampler = graph.createSampler( crg::SamplerDesc{} );
			inViewId = graph.getResources().getHandler().createViewId( data );
			auto inView = graph.createImageView( inViewId );

			for ( uint32_t level = range.baseMipLevel; level < range.baseMipLevel + range.levelCount - 1u; ++level )
			{
				ashes::WriteDescriptorSetArray writes;
				writes.push_back( ashes::WriteDescriptorSet{ eInput
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER } );
				writes.back().imageInfo.push_back( { sampler
					, inView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );

				data.info.subresourceRange.baseMipLevel++;
				data.name = imageId.data->name + "_L" + c3d::string::toMbString( data.info.subresourceRange.baseMipLevel );
				auto outViewId = graph.getResources().getHandler().createViewId( data );
				auto outView = graph.createImageView( outViewId );
				writes.push_back( ashes::WriteDescriptorSet{ eOutput
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE } );
				writes.back().imageInfo.push_back( { nullptr
					, outView
					, VK_IMAGE_LAYOUT_GENERAL } );

				auto descriptorSet = pool.createDescriptorSet( Name );
				descriptorSet->setBindings( writes );
				descriptorSet->update();

				result.emplace_back( c3d::move( descriptorSet ) );
				inView = outView;
			}

			return result;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( c3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) }
				, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_COMPUTE_BIT, 0u, uint32_t( sizeof( c3d::Point2f ) ) } } );
		}

		static ashes::ComputePipelinePtr createPipeline( c3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, c3d::ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( Name
				, ashes::ComputePipelineCreateInfo( 0u
					, c3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		static c3d::ShaderPtr createShader( c3d::RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };
			auto const G = writer.declConstant( "G", 9.81_f );

			auto pcb = writer.declPushConstantsBuffer( "MipmapsData" );
			auto invSize = pcb.declMember< sdw::Vec2 >( "invSize" );
			pcb.end();

			auto inImg = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "inImg", eInput, 0u );
			auto outImg = writer.declStorageImg< sdw::WImage2DRgba16 >( "outImg", eOutput, 0u );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 4u, 4u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto uv = writer.declLocale( "uv"
						, invSize * fma( vec2( in.globalInvocationID.xy() ), vec2( 2.0_f ), vec2( 0.5_f ) ) );
					auto filtered = writer.declLocale( "filtered"
						, vec4( 0.0_f ) );
					auto D = writer.declConstant( "D"
						, 0.5_f );

					filtered += 0.25_f * inImg.lod( uv + vec2( -D, -D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( +D, -D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( -D, +D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( +D, +D ) * invSize, 0.0_f );

					outImg.store( ivec2( in.globalInvocationID.xy() ), filtered );
				} );
			return writer.getBuilder().releaseShader();
		}

		//*****************************************************************************************

		class GenerateMipmapsPass
			: public crg::RunnablePass
		{
		public:
			GenerateMipmapsPass( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, c3d::RenderDevice const & device
				, crg::ru::Config ruConfig = {}
				, crg::RunnablePass::GetPassIndexCallback passIndex = crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
				, crg::RunnablePass::IsEnabledCallback isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) )
				: crg::RunnablePass{ pass
					, context
					, graph
					, { [this]( uint32_t index ){ doInitialise( index ); }
					, GetPipelineStateCallback( [](){ return crg::getPipelineState( c3d::PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, passIndex
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
				, { 1u } }
				, m_device{ device }
				, m_descriptorSetLayout{ genmips::createDescriptorLayout( m_device ) }
				, m_pipelineLayout{ genmips::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
				, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, c3d::makeString( Name ), genmips::createShader( device ) }
				, m_pipeline{ genmips::createPipeline( device, *m_pipelineLayout, m_shader ) }
				, m_descriptorSetPool{ m_descriptorSetLayout->createPool( crg::getMipLevels( m_pass.inouts.begin()->second->view() ) + crg::getMipLevels( m_pass.inouts.rbegin()->second->view() ) ) }
				, m_descriptorSets{ genmips::createDescriptorSets( m_graph, *m_descriptorSetPool, m_pass ) }
			{
				auto extent = getExtent( m_pass.inouts.begin()->second->view() );

				for ( size_t i = 0u; i < m_descriptorSets.size(); ++i )
				{
					m_invSizes.push_back( { 1.0f / float( extent.width )
						, 1.0f / float( extent.height ) } );
					extent.width >>= 1u;
					extent.height >>= 1u;
				}
			}

		private:
			void doInitialise( uint32_t index )
			{
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto viewAttach{ m_pass.inouts.begin()->second };
				auto viewId{ viewAttach->view( index ) };
				auto imageId{ viewId.data->image };
				auto extent = getExtent( viewId );
				auto range = viewId.data->info.subresourceRange;
				range.levelCount = getMipLevels( viewId );
				auto invSizeIt = m_invSizes.begin();
				auto neededLayoutState = getLayoutState( viewId );
				auto toLayoutState = context.getNextLayoutState( viewId );
				c3d::LayoutState shaderRead{ c3d::ImageLayout::eShaderReadOnly
					, c3d::FragmentShaderReadState };
				c3d::LayoutState shaderWrite{ c3d::ImageLayout::eGeneral
					, c3d::ComputeShaderWriteState };
				auto mipLevels = imageId.data->info.mipLevels;
				auto srcImageLayout = neededLayoutState;
				auto dstMipImageLayout = ( range.levelCount == mipLevels )
					? srcImageLayout
					: toLayoutState;
				auto format = getFormat( imageId );
				auto const aspectMask = crg::getAspectMask( format );
				c3d::ImageSubresourceRange mipSubRange{ aspectMask
					, 0u
					, 1u
					, 0u
					, 1u };
				// Transition first mip level to shader source for read in next iteration
				auto firstLayoutState = m_graph.getCurrentLayoutState( context
					, imageId
					, getImageViewType( viewId )
					, mipSubRange );
				context.memoryBarrier( commandBuffer
					, imageId
					, getImageViewType( viewId )
					, mipSubRange
					, firstLayoutState.layout
					, shaderRead );

				for ( auto & ds : m_descriptorSets )
				{
					extent.width >>= 1u;
					extent.height >>= 1u;
					++mipSubRange.baseMipLevel;
					// Transition current mip level to shader write
					context.memoryBarrier( commandBuffer
						, imageId
						, getImageViewType( viewId )
						, mipSubRange
						, c3d::ImageLayout::eUndefined
						, shaderWrite );

					// Generate mip level
					VkDescriptorSet descriptorSet = *ds;
					m_context.vkCmdBindPipeline( commandBuffer
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, *m_pipeline );
					m_context.vkCmdPushConstants( commandBuffer
						, *m_pipelineLayout
						, VK_SHADER_STAGE_COMPUTE_BIT
						, 0u
						, uint32_t( sizeof( c3d::Point2f ) )
						, &( *invSizeIt ) );
					m_context.vkCmdBindDescriptorSets( commandBuffer
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, *m_pipelineLayout
						, 0u
						, 1u
						, &descriptorSet
						, 0u
						, nullptr );
					m_context.vkCmdDispatch( commandBuffer
						, extent.width / 4u
						, extent.height / 4u
						, 1u );
					++invSizeIt;

					// Transition previous mip level to wanted output layout
					context.memoryBarrier( commandBuffer
						, imageId
						, getImageViewType( viewId )
						, { mipSubRange.aspectMask
							, mipSubRange.baseMipLevel - 1u
							, 1u
							, mipSubRange.baseArrayLayer
							, 1u }
						, shaderRead.layout
						, dstMipImageLayout );

					if ( mipSubRange.baseMipLevel == ( mipLevels - 1u ) )
					{
						// Transition final mip level to wanted output layout
						context.memoryBarrier( commandBuffer
							, imageId
							, getImageViewType( viewId )
							, mipSubRange
							, shaderWrite.layout
							, dstMipImageLayout );
					}
					else
					{
						// Transition current mip level to shader source for read in next iteration
						context.memoryBarrier( commandBuffer
							, imageId
							, getImageViewType( viewId )
							, mipSubRange
							, shaderWrite.layout
							, shaderRead );
					}
				}
			}

			bool doIsComputePass()const
			{
				return true;
			}

		private:
			c3d::LayoutState m_outputLayout;
			c3d::RenderDevice const & m_device;
			ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
			ashes::PipelineLayoutPtr m_pipelineLayout;
			c3d::ShaderModule m_shader;
			ashes::ComputePipelinePtr m_pipeline;
			ashes::DescriptorSetPoolPtr m_descriptorSetPool;
			c3d::Vector< ashes::DescriptorSetPtr > m_descriptorSets;
			c3d::Vector< c3d::Point2f > m_invSizes;
		};
	}

	//*********************************************************************************************

	void createGenerateSpecMipmapsPass( c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Texture & imageView )
	{
		auto & result = graph.createPass( "GenMips" + c3d::toUtf8( name )
			, [&device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto res = c3d::makeRawUnique< genmips::GenerateMipmapsPass >( framePass
					, context
					, graph
					, device
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		imageView.setLastAttach( result.addInOutTransfer( *imageView.getSampledLastAttach()
			, crg::Attachment::Flag::NoTransition ) );
	}


	//*********************************************************************************************
}
