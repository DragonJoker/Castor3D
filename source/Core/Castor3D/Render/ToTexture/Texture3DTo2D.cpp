#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/MixedStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/MixedStructInstanceHelper.hpp>

CU_ImplementSmartPtr( c3d, Texture3DTo2D )

#define C3D_Grid( Writer, Binding )\
	auto ubo = Writer.declUniformBuffer<>( "ubo", Binding, 0u );\
	auto grid = ubo.declMember< GridData >( "grid" );\
	ubo.end()\

namespace c3d
{
	//*********************************************************************************************

	namespace t3dto2d
	{
		struct GridData
			: public sdw::StructInstanceHelperT< "C3D_GridData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec3Field< "gridCenter" >
			, sdw::FloatField< "cellSize" >
			, sdw::UIntField< "gridSize" > >
		{
			SDW_DeclStructInstance( , GridData );

			GridData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, gridCenter{ this->getMember< "gridCenter" >() }
				, cellSize{ this->getMember< "cellSize" >() }
				, gridSize{ this->getMember< "gridSize" >() }
				, sliceIndex{ gridCenter.z() }
				, maxSlice{ cellSize }
			{
			}

			sdw::Vec3 gridCenter;
			sdw::Float cellSize;
			sdw::UInt gridSize;
			sdw::Float sliceIndex;
			sdw::Float maxSlice;
		};

		enum class Bindings : uint32_t
		{
			eGridUbo,
			eCameraUbo,
			eSource,
		};

		static Texture createDepthBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Texture const & colourView )
		{
			Texture result{ device
				, resources
				, cuT( "Texture3DToTexture2DDepth" )
				, { ImageCreateFlags::eNone
					, colourView.getExtent(), 1u, 1u
					, PixelFormat::eD32_SFLOAT
					, ImageUsageFlags::eDepthStencilAttachment }
				, { BorderColour::eFloatTransparentBlack } };
			result.create();
			return result;
		}

		static Texture createTarget( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Extent2D const & size )
		{
			Texture result{ device
				, resources
				, cuT( "Texture3DToTexture2DColor" )
				, { ImageCreateFlags::eNone
					, { size.width, size.height, 1u }, 1u, 1u
					, PixelFormat::eR8G8B8A8_UNORM
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferSrc ) }
				, { BorderColour::eFloatTransparentBlack } };
			result.create();
			return result;
		}

		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, String const & name
			, Texture const & color
			, Texture const & depth )
		{
			ashes::VkAttachmentDescriptionArray attaches{ { 0u
					, convert( color.getFormat() )
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
				, { 0u
					, convert( depth.getFormat() )
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription{ 0u
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, {}
					, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
					, {}
					, VkAttachmentReference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
					, {} } );
			ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				c3d::move( attaches ),
				c3d::move( subpasses ),
				c3d::move( dependencies ),
			};
			return device->createRenderPass( toUtf8( name )
				, c3d::move( createInfo ) );
		}

		static ashes::FrameBufferPtr createFramebuffer( ashes::RenderPass const & renderPass
			, String const & name
			, Texture const & colour
			, Texture const & depth )
		{
			ashes::VkImageViewArray fbAttaches;
			fbAttaches.emplace_back( colour.getTargetView() );
			fbAttaches.emplace_back( depth.getTargetView() );
			return renderPass.createFrameBuffer( toUtf8( name )
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, 2u
					, fbAttaches.data()
					, colour.getExtent().width
					, colour.getExtent().height
					, 1u ) );
		}

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device
			, bool isSlice
			, std::string const & suffix )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;

			if ( isSlice )
			{
				bindings.push_back( makeDescriptorSetLayoutBindingT( Bindings::eGridUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBindingT( Bindings::eSource
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
			else
			{
				bindings.push_back( makeDescriptorSetLayoutBindingT( Bindings::eGridUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBindingT( Bindings::eCameraUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_GEOMETRY_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBindingT( Bindings::eSource
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_VERTEX_BIT ) );
			}

			return device->createDescriptorSetLayout( "Texture3DTo2D" + suffix
				, c3d::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( RenderDevice const & device
			, crg::ResourcesCache & resources
			, Sampler const * sampler
			, ashes::DescriptorSetPool const & pool
			, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
			, CameraUbo const & cameraUbo
			, IntermediateView const & texture3D
			, std::string const & suffix )
		{
			ashes::WriteDescriptorSetArray writes;
			uniformBuffer.addDescriptorWriteT( writes, Bindings::eGridUbo );
			auto & context = device.makeContext();

			if ( !sampler )
			{
				cameraUbo.addDescriptorWriteT( writes, Bindings::eCameraUbo );
				writes.push_back( makeStorageImageDescriptorWrite( resources.createImageView( context, texture3D.viewId ), Bindings::eSource ) );
			}
			else
			{
				writes.push_back( makeImageViewDescriptorWrite( resources.createImageView( context, texture3D.viewId ), sampler->getSampler(), Bindings::eSource ) );
			}

			auto descriptorSet = pool.createDescriptorSet( "Texture3DTo2D" + suffix );
			descriptorSet->setBindings( c3d::move( writes ) );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout
			, std::string const & suffix )
		{
			return device->createPipelineLayout( "Texture3DTo2D" + suffix
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::GraphicsPipelinePtr createPipelineVolume( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ashes::RenderPass const & renderPass
			, ProgramModule & shader
			, Texture const & target )
		{
			auto program = makeProgramStates( device, shader );
			// Initialise the pipeline.
			VkViewport viewport{ 0.0f, 0.0f, float( target.getExtent().width ), float( target.getExtent().height ), 0.0f, 1.0f };
			VkRect2D scissor{ 0, 0, target.getExtent().width, target.getExtent().height };
			return device->createPipeline( "Texture3DTo2DVolume"
				, ashes::GraphicsPipelineCreateInfo( 0u
					, program
					, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
					, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
					, ashes::nullopt
					, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
					, ashes::PipelineMultisampleStateCreateInfo{}
					, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_TRUE, VK_COMPARE_OP_GREATER }
					, RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
					, ashes::nullopt
					, layout
					, static_cast< VkRenderPass const & >( renderPass ) ) );
		}

		static ashes::GraphicsPipelinePtr createPipelineSlice( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ashes::RenderPass const & renderPass
			, ProgramModule & shader
			, Texture const & target )
		{
			auto program = makeProgramStates( device, shader );
			// Initialise the pipeline.
			VkViewport viewport{ 0.0f, 0.0f, float( target.getExtent().width ), float( target.getExtent().height ), 0.0f, 1.0f };
			VkRect2D scissor{ 0, 0, target.getExtent().width, target.getExtent().height };
			return device->createPipeline( "Texture3DTo2DSlice"
				, ashes::GraphicsPipelineCreateInfo( 0u
					, program
					, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
					, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
					, ashes::nullopt
					, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
					, ashes::PipelineMultisampleStateCreateInfo{}
					, ashes::PipelineDepthStencilStateCreateInfo{}
					, RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
					, ashes::nullopt
					, layout
					, static_cast< VkRenderPass const & >( renderPass ) ) );
		}

		static CommandsSemaphore createCommandBuffer( RenderDevice const & device
			, QueueData const & queueData
			, crg::ResourcesCache & resources
			, ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::GraphicsPipeline const & pipeline
			, ashes::DescriptorSet const & descriptorSet
			, IntermediateView const & view
			, Sampler const * sampler
			, std::string const & suffix )
		{
			auto & context = device.makeContext();
			auto textureSize = getExtent( view.viewId ).width;
			CommandsSemaphore result{ device, queueData, cuT( "Texture3DTo2D" + suffix ) };
			auto const & cmd = *result.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock( { "Texture3D To Texture2D"
				, makeFloatArray( c3d::getEngine( device ).getNextRainbowColour() ) } );

			if ( sampler )
			{
				cmd.memoryBarrier( ashes::getStageMask( convert( view.layout ) )
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, view.layout
						, ImageLayout::eShaderReadOnly
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}
			else if ( view.layout != ImageLayout::eGeneral )
			{
				cmd.memoryBarrier( ashes::getStageMask( convert( view.layout ) )
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, view.layout
						, ImageLayout::eGeneral
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.beginRenderPass( renderPass
				, frameBuffer
				, { convert( ClearValue{ opaqueBlackClearColor } ), convert( ClearValue{ defaultClearDepthStencil } ) }
				, VK_SUBPASS_CONTENTS_INLINE );
			cmd.bindPipeline( pipeline );
			cmd.bindDescriptorSet( descriptorSet, pipelineLayout );

			if ( sampler )
			{
				cmd.draw( 3u );
			}
			else
			{
				cmd.draw( textureSize * textureSize * textureSize );
			}

			cmd.endRenderPass();

			if ( sampler )
			{
				if ( view.layout != ImageLayout::eShaderReadOnly
					&& view.layout != ImageLayout::eUndefined )
				{
					cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, ashes::getStageMask( convert( view.layout ) )
						, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
							, view.viewId.data->info.subresourceRange
							, ImageLayout::eShaderReadOnly
							, view.layout
							, VK_QUEUE_FAMILY_IGNORED
							, VK_QUEUE_FAMILY_IGNORED ) );
				}
			}
			else if ( view.layout != ImageLayout::eGeneral
				&& view.layout != ImageLayout::eUndefined )
			{
				cmd.memoryBarrier( VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, ashes::getStageMask( convert( view.layout ) )
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, ImageLayout::eGeneral
						, view.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.endDebugBlock();
			cmd.end();

			return result;
		}

		template< sdw::var::Flag FlagT >
		using SurfaceStrucT = sdw::MixedStructInstanceHelperT < FlagT
			, "C3D_T3DT3DSurface"
			, sdw::type::MemoryLayout::eStd430
			, sdw::IOVec4Field< "voxelColour", 0u > >;

		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: SurfaceStrucT< FlagT >
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: SurfaceStrucT< FlagT >{ writer, c3d::move( expr ), enabled }
			{
			}

			auto voxelColour()const { return this->template getMember< "voxelColour" >(); }
		};

		template< ast::type::ImageFormat FormatT >
		static ShaderPtr getProgramVolume( RenderSystem const & renderSystem )
		{
			sdw::TraditionalGraphicsWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			shader::Utils utils{ writer };

			C3D_Grid( writer, Bindings::eGridUbo );
			C3D_Camera( writer, Bindings::eCameraUbo, 0u );
			auto inSource( writer.declStorageImg< FormatT, RImg3D >( "inSource", Bindings::eSource, 0u ) );

			// Creates a unit cube triangle strip from just vertex ID (14 vertices)
			auto createCube = writer.implementFunction< sdw::Vec3 >( "createCube"
				, [&writer]( sdw::UInt const & vertexID )
				{
					auto b = writer.declLocale( "b"
						, 1_u << vertexID );
					writer.returnStmt( vec3( writer.ternary( ( 0x287a_u & b ) != 0u, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x02af_u & b ) != 0u, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x31e3_u & b ) != 0u, 1.0_f, 0.0_f ) ) );
				}
				, sdw::InUInt{ writer, "vertexID" } );

			writer.implementEntryPointT< sdw::VoidT, SurfaceT >( [&writer, &grid, &utils, &inSource]( sdw::VertexIn const & in
				, sdw::VertexOutT< SurfaceT > out )
				{
					auto coord = writer.declLocale( "coord"
						, utils.unflatten( writer.cast< sdw::UInt >( in.vertexIndex )
							, uvec3( grid.gridSize ) ) );
					out.vtx.position = vec4( vec3( coord ), 1.0f );

					out.voxelColour() = shader::makeVec4( writer, inSource.load( ivec3( coord ) ) );
				} );

			writer.implementEntryPointT< 14u, sdw::PointListT< SurfaceT >, sdw::TriangleStreamT< SurfaceT > >( [&writer, &grid, &createCube, &c3d_cameraData]( sdw::GeometryIn const &
				, sdw::PointListT< SurfaceT > const & list
				, sdw::TriangleStreamT< SurfaceT > out )
				{
					sdwIF( writer, list[0].voxelColour().a() > 0.0f )
					{
						sdwFOR( writer, sdw::UInt, i, 0_u, i < 14_u, ++i )
						{
							// [0, 1] => [0, 2] (y => [-1, 1])
							auto cubeVtxPos = writer.declLocale( "cubeVtxPos"
								, ( createCube( i ) - vec3( 0.0_f, 1.0f, 0.0f ) ) * 2.0f );

							// [0, gridSize]
							auto pos = writer.declLocale( "pos"
								, list[0].vtx.position.xyz() );
							// [0, gridSize] => [0, 1] => [-1, 1]
							pos = pos / writer.cast< sdw::Float >( grid.gridSize ) * 2.0f - 1.0f;
							pos.y() = -pos.y();
							// [-1, 1] => [-gridSize, gridSize]
							pos *= writer.cast< sdw::Float >( grid.gridSize );
							// Offset by cube position
							pos += cubeVtxPos;
							pos *= ( writer.cast< sdw::Float >( grid.gridSize ) * ( 1.0_f / grid.cellSize ) ) / writer.cast< sdw::Float >( grid.gridSize );

							out.voxelColour() = list[0].voxelColour();
							out.vtx.position = c3d_cameraData.worldToCurProj( vec4( pos, 1.0f ) );

							out.append();
						}
						sdwROF

						out.restartStrip();
					}
					sdwFI
				} );

			writer.implementEntryPointT< SurfaceT, shader::Colour4FT >( []( sdw::FragmentInT< SurfaceT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					out.colour() = vec4( in.voxelColour().rgb(), 1.0_f );
				} );
			return writer.getBuilder().releaseShader();
		}

		static ShaderPtr getProgramSlice( RenderSystem const & renderSystem )
		{
			sdw::TraditionalGraphicsWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			shader::Utils utils{ writer };

			C3D_Grid( writer, Bindings::eGridUbo );
			auto inSource( writer.declCombinedImg< Img3DRgba >( "inSource", Bindings::eSource, 0u ) );

			writer.implementEntryPointT< sdw::VoidT, shader::Uv2FT >( []( sdw::VertexIn const & in
				, sdw::VertexOutT< shader::Uv2FT > out )
				{
					out.uv() = vec2( ( in.vertexIndex << 1 ) & 2, in.vertexIndex & 2 );
					out.vtx.position = vec4( out.uv() * 2.0f - 1.0f, 0.0f, 1.0f );
				} );

			writer.implementEntryPointT< shader::Uv2FT, shader::Colour4FT >( [&grid, inSource]( sdw::FragmentInT< shader::Uv2FT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					out.colour() = inSource.lod( vec3( in.uv(), grid.sliceIndex / grid.maxSlice ), 0.0_f );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	ast::type::ImageFormat getImageFormat( PixelFormat format )
	{
		switch ( format )
		{
		case PixelFormat::eR8_UNORM:
		case PixelFormat::eBC4_UNORM_BLOCK:
			return ast::type::ImageFormat::eR8Unorm;
		case PixelFormat::eR8_SNORM:
		case PixelFormat::eBC4_SNORM_BLOCK:
			return ast::type::ImageFormat::eR8Snorm;
		case PixelFormat::eR8_UINT:
			return ast::type::ImageFormat::eR8u;
		case PixelFormat::eR8_SINT:
		case PixelFormat::eS8_UINT:
			return ast::type::ImageFormat::eR8i;
		case PixelFormat::eR8_SRGB:
			return ast::type::ImageFormat::eR8Unorm;
		case PixelFormat::eR8G8_UNORM:
		case PixelFormat::eR8G8_SRGB:
		case PixelFormat::eBC5_UNORM_BLOCK:
			return ast::type::ImageFormat::eRg8Unorm;
		case PixelFormat::eR8G8_SNORM:
		case PixelFormat::eBC5_SNORM_BLOCK:
			return ast::type::ImageFormat::eRg8Snorm;
		case PixelFormat::eR8G8_UINT:
			return ast::type::ImageFormat::eRg8u;
		case PixelFormat::eR8G8_SINT:
			return ast::type::ImageFormat::eRg8i;
		case PixelFormat::eR8G8B8_UNORM:
		case PixelFormat::eR8G8B8_SRGB:
		case PixelFormat::eB8G8R8_UNORM:
		case PixelFormat::eB8G8R8_SRGB:
		case PixelFormat::eR8G8B8A8_UNORM:
		case PixelFormat::eR8G8B8A8_SRGB:
		case PixelFormat::eB8G8R8A8_UNORM:
		case PixelFormat::eB8G8R8A8_SRGB:
		case PixelFormat::eA8B8G8R8_UNORM:
		case PixelFormat::eA8B8G8R8_SRGB:
			return ast::type::ImageFormat::eRgba8Unorm;
		case PixelFormat::eR8G8B8_SNORM:
		case PixelFormat::eB8G8R8_SNORM:
		case PixelFormat::eR8G8B8A8_SNORM:
		case PixelFormat::eB8G8R8A8_SNORM:
		case PixelFormat::eA8B8G8R8_SNORM:
			return ast::type::ImageFormat::eRgba8Snorm;
		case PixelFormat::eR8G8B8_UINT:
		case PixelFormat::eB8G8R8_UINT:
		case PixelFormat::eR8G8B8A8_UINT:
		case PixelFormat::eB8G8R8A8_UINT:
		case PixelFormat::eA8B8G8R8_UINT:
			return ast::type::ImageFormat::eRgba8u;
		case PixelFormat::eR8G8B8_SINT:
		case PixelFormat::eB8G8R8_SINT:
		case PixelFormat::eR8G8B8A8_SINT:
		case PixelFormat::eB8G8R8A8_SINT:
		case PixelFormat::eA8B8G8R8_SINT:
			return ast::type::ImageFormat::eRgba8i;
		case PixelFormat::eA2R10G10B10_UNORM:
		case PixelFormat::eA2B10G10R10_UNORM:
			return ast::type::ImageFormat::eRgb10A2Unorm;
		case PixelFormat::eA2R10G10B10_UINT:
		case PixelFormat::eA2B10G10R10_UINT:
			return ast::type::ImageFormat::eRgb10A2Unorm;
		case PixelFormat::eR16_UNORM:
		case PixelFormat::eD16_UNORM:
		case PixelFormat::eEAC_R11_UNORM_BLOCK:
			return ast::type::ImageFormat::eR16Unorm;
		case PixelFormat::eR16_SNORM:
			return ast::type::ImageFormat::eR16Snorm;
		case PixelFormat::eR16_UINT:
			return ast::type::ImageFormat::eR16u;
		case PixelFormat::eR16_SINT:
			return ast::type::ImageFormat::eR16i;
		case PixelFormat::eR16_SFLOAT:
			return ast::type::ImageFormat::eR16f;
		case PixelFormat::eR16G16_UNORM:
		case PixelFormat::eEAC_R11G11_UNORM_BLOCK:
			return ast::type::ImageFormat::eRg16Unorm;
		case PixelFormat::eR16G16_SNORM:
		case PixelFormat::eEAC_R11G11_SNORM_BLOCK:
		case PixelFormat::eEAC_R11_SNORM_BLOCK:
			return ast::type::ImageFormat::eRg16Snorm;
		case PixelFormat::eR16G16_UINT:
			return ast::type::ImageFormat::eRg16u;
		case PixelFormat::eR16G16_SINT:
			return ast::type::ImageFormat::eRg16i;
		case PixelFormat::eR16G16_SFLOAT:
			return ast::type::ImageFormat::eRg16f;
		case PixelFormat::eR16G16B16_UNORM:
		case PixelFormat::eR16G16B16A16_UNORM:
			return ast::type::ImageFormat::eRgba16Unorm;
		case PixelFormat::eR16G16B16_SNORM:
		case PixelFormat::eR16G16B16A16_SNORM:
		case PixelFormat::eA2R10G10B10_SNORM:
		case PixelFormat::eA2B10G10R10_SNORM:
			return ast::type::ImageFormat::eRgba16Snorm;
		case PixelFormat::eR16G16B16_UINT:
		case PixelFormat::eR16G16B16A16_UINT:
			return ast::type::ImageFormat::eRgba16u;
		case PixelFormat::eR16G16B16_SINT:
		case PixelFormat::eR16G16B16A16_SINT:
		case PixelFormat::eA2R10G10B10_SINT:
		case PixelFormat::eA2B10G10R10_SINT:
			return ast::type::ImageFormat::eRgba16i;
		case PixelFormat::eR16G16B16_SFLOAT:
		case PixelFormat::eR16G16B16A16_SFLOAT:
		case PixelFormat::eE5B9G9R9_UFLOAT:
			return ast::type::ImageFormat::eRgba16f;
		case PixelFormat::eR32_UINT:
		case PixelFormat::eR64_UINT:
			return ast::type::ImageFormat::eR32u;
		case PixelFormat::eR32_SINT:
		case PixelFormat::eR64_SINT:
			return ast::type::ImageFormat::eR32i;
		case PixelFormat::eR32_SFLOAT:
		case PixelFormat::eR64_SFLOAT:
		case PixelFormat::eD32_SFLOAT:
		case PixelFormat::eX8_D24_UNORM:
			return ast::type::ImageFormat::eR32f;
		case PixelFormat::eR32G32_UINT:
		case PixelFormat::eR64G64_UINT:
			return ast::type::ImageFormat::eRg32u;
		case PixelFormat::eR32G32_SINT:
		case PixelFormat::eR64G64_SINT:
		case PixelFormat::eD16_UNORM_S8_UINT:
		case PixelFormat::eD24_UNORM_S8_UINT:
			return ast::type::ImageFormat::eRg32i;
		case PixelFormat::eR32G32_SFLOAT:
		case PixelFormat::eR64G64_SFLOAT:
		case PixelFormat::eD32_SFLOAT_S8_UINT:
			return ast::type::ImageFormat::eRg32f;
		case PixelFormat::eR32G32B32_UINT:
		case PixelFormat::eR32G32B32A32_UINT:
		case PixelFormat::eR64G64B64_UINT:
		case PixelFormat::eR64G64B64A64_UINT:
			return ast::type::ImageFormat::eRgba32u;
		case PixelFormat::eR32G32B32_SINT:
		case PixelFormat::eR32G32B32A32_SINT:
		case PixelFormat::eR64G64B64_SINT:
		case PixelFormat::eR64G64B64A64_SINT:
			return ast::type::ImageFormat::eRgba32i;
		case PixelFormat::eR32G32B32_SFLOAT:
		case PixelFormat::eR32G32B32A32_SFLOAT:
		case PixelFormat::eR64G64B64_SFLOAT:
		case PixelFormat::eR64G64B64A64_SFLOAT:
		case PixelFormat::eBC6H_UFLOAT_BLOCK:
		case PixelFormat::eBC6H_SFLOAT_BLOCK:
			return ast::type::ImageFormat::eRgba32f;
		case PixelFormat::eB10G11R11_UFLOAT:
			return ast::type::ImageFormat::eR11fG11fB10f;
		case PixelFormat::eBC1_RGB_UNORM_BLOCK:
		case PixelFormat::eBC1_RGB_SRGB_BLOCK:
		case PixelFormat::eBC1_RGBA_UNORM_BLOCK:
		case PixelFormat::eBC1_RGBA_SRGB_BLOCK:
		case PixelFormat::eBC2_UNORM_BLOCK:
		case PixelFormat::eBC2_SRGB_BLOCK:
		case PixelFormat::eBC3_UNORM_BLOCK:
		case PixelFormat::eBC3_SRGB_BLOCK:
		case PixelFormat::eBC7_UNORM_BLOCK:
		case PixelFormat::eBC7_SRGB_BLOCK:
		case PixelFormat::eETC2_R8G8B8_UNORM_BLOCK:
		case PixelFormat::eETC2_R8G8B8_SRGB_BLOCK:
		case PixelFormat::eETC2_R8G8B8A1_UNORM_BLOCK:
		case PixelFormat::eETC2_R8G8B8A1_SRGB_BLOCK:
		case PixelFormat::eETC2_R8G8B8A8_UNORM_BLOCK:
		case PixelFormat::eETC2_R8G8B8A8_SRGB_BLOCK:
		case PixelFormat::eASTC_4x4_UNORM_BLOCK:
		case PixelFormat::eASTC_4x4_SRGB_BLOCK:
		case PixelFormat::eASTC_5x4_UNORM_BLOCK:
		case PixelFormat::eASTC_5x4_SRGB_BLOCK:
		case PixelFormat::eASTC_5x5_UNORM_BLOCK:
		case PixelFormat::eASTC_5x5_SRGB_BLOCK:
		case PixelFormat::eASTC_6x5_UNORM_BLOCK:
		case PixelFormat::eASTC_6x5_SRGB_BLOCK:
		case PixelFormat::eASTC_6x6_UNORM_BLOCK:
		case PixelFormat::eASTC_6x6_SRGB_BLOCK:
		case PixelFormat::eASTC_8x5_UNORM_BLOCK:
		case PixelFormat::eASTC_8x5_SRGB_BLOCK:
		case PixelFormat::eASTC_8x6_UNORM_BLOCK:
		case PixelFormat::eASTC_8x6_SRGB_BLOCK:
		case PixelFormat::eASTC_8x8_UNORM_BLOCK:
		case PixelFormat::eASTC_8x8_SRGB_BLOCK:
		case PixelFormat::eASTC_10x5_UNORM_BLOCK:
		case PixelFormat::eASTC_10x5_SRGB_BLOCK:
		case PixelFormat::eASTC_10x6_UNORM_BLOCK:
		case PixelFormat::eASTC_10x6_SRGB_BLOCK:
		case PixelFormat::eASTC_10x8_UNORM_BLOCK:
		case PixelFormat::eASTC_10x8_SRGB_BLOCK:
		case PixelFormat::eASTC_10x10_UNORM_BLOCK:
		case PixelFormat::eASTC_10x10_SRGB_BLOCK:
		case PixelFormat::eASTC_12x10_UNORM_BLOCK:
		case PixelFormat::eASTC_12x10_SRGB_BLOCK:
		case PixelFormat::eASTC_12x12_UNORM_BLOCK:
		case PixelFormat::eASTC_12x12_SRGB_BLOCK:
		default:
			return ast::type::ImageFormat::eRgba8Unorm;
		}
	}

	//*********************************************************************************************

	Texture3DTo2D::Texture3DToScreen::Texture3DToScreen( RenderDevice const & device
		, QueueData const & queueData
		, crg::ResourcesCache & resources
		, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
		, CameraUbo const & cameraUbo
		, IntermediateView const & texture3D
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorSetPool
		, ashes::FrameBuffer const & frameBuffer
		, ashes::PipelineLayout const & pipelineLayout
		, ashes::GraphicsPipeline const & pipeline
		, Sampler const * sampler )
		: descriptorSet{ t3dto2d::createDescriptorSet( device, resources, sampler, descriptorSetPool, uniformBuffer, cameraUbo, texture3D, "Screen" ) }
		, commands{ t3dto2d::createCommandBuffer( device, queueData, resources, renderPass, frameBuffer, pipelineLayout, pipeline, *descriptorSet, texture3D, sampler, "Screen" ) }
	{
	}

	//*********************************************************************************************

	Texture3DTo2D::Texture3DTo2D( RenderDevice const & device
		, crg::ResourcesCache & resources
		, Extent2D const & size
		, CameraUbo const & cameraUbo )
		: m_device{ device }
		, m_resources{ resources }
		, m_cameraUbo{ cameraUbo }
		, m_target{ t3dto2d::createTarget( device, resources, size ) }
		, m_depthBuffer{ t3dto2d::createDepthBuffer( device, resources, m_target ) }
		, m_uniformBuffer{ device }
		, m_renderPass{ t3dto2d::createRenderPass( device, cuT( "Texture3DTo2D" ), m_target, m_depthBuffer ) }
		, m_frameBuffer{ t3dto2d::createFramebuffer( *m_renderPass, cuT( "Texture3DTo2D" ), m_target, m_depthBuffer ) }
		, m_sampler{ makeUnique< Sampler >( cuT( "Slice" )
			, c3d::getEngine( device )
			, ashes::SamplerCreateInfo{ 0u
				, VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, 0.0f
				, VK_FALSE
				, 0.0f
				, VK_FALSE
				, VK_COMPARE_OP_ALWAYS
				, 0.0f
				, 1.0f } ) }

		, m_descriptorSetLayoutVolume{ t3dto2d::createDescriptorLayout( device, false, "Volume" ) }
		, m_pipelineLayoutVolume{ t3dto2d::createPipelineLayout( device, *m_descriptorSetLayoutVolume, "Volume" ) }
		, m_descriptorSetLayoutSlice{ t3dto2d::createDescriptorLayout( device, true, "Slice" ) }
		, m_pipelineLayoutSlice{ t3dto2d::createPipelineLayout( device, *m_descriptorSetLayoutSlice, "Slice" ) }
		, m_shaderSlice{ cuT( "Texture3DTo2D_Slice" ), t3dto2d::getProgramSlice( m_device.renderSystem ) }
		, m_pipelineSlice{ t3dto2d::createPipelineSlice( m_device, *m_pipelineLayoutSlice, *m_renderPass, m_shaderSlice, m_target ) }
	{
		m_sampler->initialise( device );
	}

	Texture3DTo2D::~Texture3DTo2D()noexcept
	{
		m_sampler->cleanup();
		m_depthBuffer.destroy();
		m_target.destroy();
	}

	void Texture3DTo2D::createPasses( QueueData const & queueData
		, IntermediateViewArray intermediates )
	{
		m_textures = c3d::move( intermediates );
		initialise( queueData );
	}

	void Texture3DTo2D::initialise( QueueData const & queueData )
	{
		m_descriptorSetPoolVolume = m_descriptorSetLayoutVolume->createPool( uint32_t( m_textures.size() ) );
		m_descriptorSetPoolSlice = m_descriptorSetLayoutSlice->createPool( uint32_t( m_textures.size() ) );

		for ( auto & intermediate : m_textures )
		{
			if ( intermediate.viewId.data->image.data->info.imageType == ImageType::e3D )
			{
				if ( intermediate.factors.isSlice )
				{
					m_texture3DToScreen.emplace_back( m_device
						, queueData
						, m_resources
						, m_uniformBuffer.getUbo()
						, m_cameraUbo
						, intermediate
						, *m_renderPass
						, *m_descriptorSetPoolSlice
						, *m_frameBuffer
						, *m_pipelineLayoutSlice
						, *m_pipelineSlice
						, m_sampler.get() );
				}
				else
				{
					ast::type::ImageFormat format = getImageFormat( getFormat( intermediate.viewId ) );
					auto [it, inserted] = m_pipelineVolume.try_emplace( format, PipelineProgram{ {}, {} } );
					if ( inserted )
					{
						switch ( format )
						{
						case ast::type::ImageFormat::eRgba32f: it->second.shader = { cuT( "Texture3DTo2D_Rgba32f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba32f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba16f: it->second.shader = { cuT( "Texture3DTo2D_Rgba16f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba16f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg32f: it->second.shader = { cuT( "Texture3DTo2D_Rg32f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg32f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg16f: it->second.shader = { cuT( "Texture3DTo2D_Rg16f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg16f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR32f: it->second.shader = { cuT( "Texture3DTo2D_R32f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR32f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR16f: it->second.shader = { cuT( "Texture3DTo2D_R16f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR16f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR11fG11fB10f: it->second.shader = { cuT( "Texture3DTo2D_R11fG11fB10f" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR11fG11fB10f >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba32i: it->second.shader = { cuT( "Texture3DTo2D_Rgba32i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba32i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba16i: it->second.shader = { cuT( "Texture3DTo2D_Rgba16i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba16i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba8i: it->second.shader = { cuT( "Texture3DTo2D_Rgba8i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba8i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg32i: it->second.shader = { cuT( "Texture3DTo2D_Rg32i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg32i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg16i: it->second.shader = { cuT( "Texture3DTo2D_Rg16i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg16i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg8i: it->second.shader = { cuT( "Texture3DTo2D_Rg8i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg8i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR32i: it->second.shader = { cuT( "Texture3DTo2D_R32i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR32i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR16i: it->second.shader = { cuT( "Texture3DTo2D_R16i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR16i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR8i: it->second.shader = { cuT( "Texture3DTo2D_R8i" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR8i >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba32u: it->second.shader = { cuT( "Texture3DTo2D_Rgba32u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba32u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba16u: it->second.shader = { cuT( "Texture3DTo2D_Rgba16u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba16u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba8u: it->second.shader = { cuT( "Texture3DTo2D_Rgba8u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba8u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg32u: it->second.shader = { cuT( "Texture3DTo2D_Rg32u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg32u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg16u: it->second.shader = { cuT( "Texture3DTo2D_Rg16u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg16u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg8u: it->second.shader = { cuT( "Texture3DTo2D_Rg8u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg8u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR32u: it->second.shader = { cuT( "Texture3DTo2D_R32u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR32u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR16u: it->second.shader = { cuT( "Texture3DTo2D_R16u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR16u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR8u: it->second.shader = { cuT( "Texture3DTo2D_R8u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR8u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgb10A2u: it->second.shader = { cuT( "Texture3DTo2D_Rgb10A2u" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgb10A2u >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba16Snorm: it->second.shader = { cuT( "Texture3DTo2D_Rgba16Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba16Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba8Snorm: it->second.shader = { cuT( "Texture3DTo2D_Rgba8Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba8Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg16Snorm: it->second.shader = { cuT( "Texture3DTo2D_Rg16Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg16Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg8Snorm: it->second.shader = { cuT( "Texture3DTo2D_Rg8Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg8Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR16Snorm: it->second.shader = { cuT( "Texture3DTo2D_R16Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR16Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR8Snorm: it->second.shader = { cuT( "Texture3DTo2D_R8Snorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR8Snorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba16Unorm: it->second.shader = { cuT( "Texture3DTo2D_Rgba16Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba16Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgba8Unorm: it->second.shader = { cuT( "Texture3DTo2D_Rgba8Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgba8Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg16Unorm: it->second.shader = { cuT( "Texture3DTo2D_Rg16Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg16Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRg8Unorm: it->second.shader = { cuT( "Texture3DTo2D_Rg8Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRg8Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR16Unorm: it->second.shader = { cuT( "Texture3DTo2D_R16Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR16Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eR8Unorm: it->second.shader = { cuT( "Texture3DTo2D_R8Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eR8Unorm >( m_device.renderSystem ) }; break;
						case ast::type::ImageFormat::eRgb10A2Unorm: it->second.shader = { cuT( "Texture3DTo2D_Rgb10A2Unorm" ), t3dto2d::getProgramVolume< ast::type::ImageFormat::eRgb10A2Unorm >( m_device.renderSystem ) }; break;
						default:
							CU_Failure( "Unsupported ImageFormat" );
							break;
						}
						if ( !it->second.shader.shader )
							continue;
						it->second.pipeline = t3dto2d::createPipelineVolume( m_device, *m_pipelineLayoutVolume, *m_renderPass, it->second.shader, m_target );
					}

					m_texture3DToScreen.emplace_back( m_device
						, queueData
						, m_resources
						, m_uniformBuffer.getUbo()
						, m_cameraUbo
						, intermediate
						, *m_renderPass
						, *m_descriptorSetPoolVolume
						, *m_frameBuffer
						, *m_pipelineLayoutVolume
						, *it->second.pipeline
						, nullptr );
				}
			}
			else
			{
				m_texture3DToScreen.emplace_back();
			}
		}
	}

	void Texture3DTo2D::update( CpuUpdater & updater )
	{
		m_index = updater.combineIndex;

		if ( m_textures[m_index].factors.isSlice )
		{
			auto data = m_uniformBuffer.getData();
			data.gridCenterCellSize = Point4f{ 0.0f
				, 0.0f
				, m_textures[m_index].factors.slice
				, m_textures[m_index].viewId.data->image.data->info.extent.depth - 1u };
			m_uniformBuffer.setData( c3d::move( data ) );
		}
		else if ( updater.cellSize != 0.0f )
		{
			auto data = m_uniformBuffer.getData();
			data.gridCenterCellSize = Point4f{ updater.gridCenter->x
				, updater.gridCenter->y
				, updater.gridCenter->z
				, updater.cellSize };
			data.gridSize = getExtent( m_textures[m_index].viewId ).width;
			m_uniformBuffer.setData( c3d::move( data ) );
		}
	}

	SemaphoreWait Texture3DTo2D::render( ashes::Queue const & queue
		, SemaphoreWait const & toWait )
	{
		auto result = toWait;

		if ( m_texture3DToScreen[m_index].commands.commandBuffer )
		{
			result.semaphore = m_texture3DToScreen[m_index].commands.submit( queue, result );
		}

		return result;
	}

	void Texture3DTo2D::render( ashes::Queue const & queue
		, ashes::VkSemaphoreArray & semaphores
		, ashes::VkPipelineStageFlagsArray & stages )
	{
		if ( m_texture3DToScreen[m_index].commands.commandBuffer )
		{
			m_texture3DToScreen[m_index].commands.submit( queue, semaphores, stages );
		}
	}

	//*********************************************************************************************
}
