#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
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

#include <ShaderWriter/Source.hpp>

#include <algorithm>

CU_ImplementSmartPtr( castor3d, Texture3DTo2D )

#define UBO_GRID( Writer, Binding )\
	auto ubo = Writer.declUniformBuffer<>( "ubo", Binding, 0u );\
	auto gridCenterCellSize = ubo.declMember< sdw::Vec4 >( "gridCenter" );\
	auto gridSize = ubo.declMember< sdw::UInt >( "gridSize" );\
	ubo.end();\
	auto gridCenter = gridCenterCellSize.xyz();\
	auto cellSize = gridCenterCellSize.w();\
	auto sliceIndex = gridCenterCellSize.z();\
	auto maxSlice = gridCenterCellSize.w()

namespace castor3d
{
	//*********************************************************************************************

	namespace t3dto2d
	{
		enum IDs : uint32_t
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
				, "Texture3DToTexture2DDepth"
				, 0u
				, colourView.getExtent()
				, 1u
				, 1u
				, VK_FORMAT_D32_SFLOAT
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK };
			result.create();
			return result;
		}

		static Texture createTarget( RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkExtent2D const & size )
		{
			Texture result{ device
				, resources
				, "Texture3DToTexture2DColor"
				, 0u
				, { size.width, size.height, 1u }
				, 1u
				, 1u
				, VK_FORMAT_R8G8B8A8_UNORM
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK };
			result.create();
			return result;
		}

		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, castor::String const & name
			, Texture const & color
			, Texture const & depth )
		{
			ashes::VkAttachmentDescriptionArray attaches{ { 0u
					, color.getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
				, { 0u
					, depth.getFormat()
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
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			return device->createRenderPass( name
				, std::move( createInfo ) );
		}

		static ashes::FrameBufferPtr createFramebuffer( ashes::RenderPass const & renderPass
			, castor::String const & name
			, Texture const & colour
			, Texture const & depth )
		{
			ashes::VkImageViewArray fbAttaches;
			fbAttaches.emplace_back( colour.targetView );
			fbAttaches.emplace_back( depth.targetView );
			return renderPass.createFrameBuffer( name
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, 2u
					, fbAttaches.data()
					, colour.getExtent().width
					, colour.getExtent().height
					, 1u ) );
		}

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device
			, bool isSlice )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;

			if ( isSlice )
			{
				bindings.push_back( makeDescriptorSetLayoutBinding( eGridUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBinding( eSource
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) );
			}
			else
			{
				bindings.push_back( makeDescriptorSetLayoutBinding( eGridUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBinding( eCameraUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_GEOMETRY_BIT ) );
				bindings.push_back( makeDescriptorSetLayoutBinding( eSource
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_VERTEX_BIT ) );
			}

			return device->createDescriptorSetLayout( "Texture3DTo2D"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( RenderDevice const & device
			, crg::ResourcesCache & resources
			, SamplerRPtr sampler
			, ashes::DescriptorSetPool const & pool
			, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
			, CameraUbo const & cameraUbo
			, IntermediateView const & texture3D )
		{
			auto descriptorSet = pool.createDescriptorSet( "Texture3DTo2D" );
			uniformBuffer.createSizedBinding( *descriptorSet
				, pool.getLayout().getBinding( eGridUbo ) );
			auto & context = device.makeContext();

			if ( !sampler )
			{
				cameraUbo.createSizedBinding( *descriptorSet
					, pool.getLayout().getBinding( eCameraUbo ) );
				descriptorSet->createBinding( pool.getLayout().getBinding( eSource )
					, resources.createImageView( context, texture3D.viewId ) );
			}
			else
			{
				descriptorSet->createBinding( pool.getLayout().getBinding( eSource )
					, resources.createImageView( context, texture3D.viewId )
					, sampler->getSampler() );
			}

			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "Texture3DTo2D"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::GraphicsPipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ashes::RenderPass const & renderPass
			, ShaderModule & vertexShader
			, ShaderModule & geometryShader
			, ShaderModule & pixelShader
			, Texture const & target )
		{
			ashes::PipelineShaderStageCreateInfoArray program;
			program.push_back( makeShaderState( device, vertexShader ) );
			program.push_back( makeShaderState( device, geometryShader ) );
			program.push_back( makeShaderState( device, pixelShader ) );
			// Initialise the pipeline.
			VkViewport viewport{ 0.0f, 0.0f, float( target.getExtent().width ), float( target.getExtent().height ), 0.0f, 1.0f };
			VkRect2D scissor{ 0, 0, target.getExtent().width, target.getExtent().height };
			return device->createPipeline( "Texture3DTo2D"
				, ashes::GraphicsPipelineCreateInfo( 0u
					, program
					, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
					, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
					, ashes::nullopt
					, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT }
					, ashes::PipelineMultisampleStateCreateInfo{}
					, ashes::PipelineDepthStencilStateCreateInfo{}
					, RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
					, ashes::nullopt
					, layout
					, static_cast< VkRenderPass const & >( renderPass ) ) );
		}

		static ashes::GraphicsPipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ashes::RenderPass const & renderPass
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader
			, Texture const & target )
		{
			ashes::PipelineShaderStageCreateInfoArray program;
			program.push_back( makeShaderState( device, vertexShader ) );
			program.push_back( makeShaderState( device, pixelShader ) );
			// Initialise the pipeline.
			VkViewport viewport{ 0.0f, 0.0f, float( target.getExtent().width ), float( target.getExtent().height ), 0.0f, 1.0f };
			VkRect2D scissor{ 0, 0, target.getExtent().width, target.getExtent().height };
			return device->createPipeline( "Texture3DTo2D"
				, ashes::GraphicsPipelineCreateInfo( 0u
					, program
					, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
					, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
					, ashes::nullopt
					, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
					, ashes::PipelineMultisampleStateCreateInfo{}
					, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE }
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
			, SamplerRPtr sampler )
		{
			auto & context = device.makeContext();
			auto textureSize = getExtent( view.viewId ).width;
			CommandsSemaphore result{ device, queueData, "Texture3DTo2D" };
			auto & cmd = *result.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock( { "Texture3D To Texture2D"
				, makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ) } );

			if ( sampler )
			{
				cmd.memoryBarrier( ashes::getStageMask( view.layout )
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, view.layout
						, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}
			else if ( view.layout != VK_IMAGE_LAYOUT_GENERAL )
			{
				cmd.memoryBarrier( ashes::getStageMask( view.layout )
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, view.layout
						, VK_IMAGE_LAYOUT_GENERAL
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.beginRenderPass( renderPass
				, frameBuffer
				, { opaqueBlackClearColor, defaultClearDepthStencil }
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
				if ( view.layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					&& view.layout != VK_IMAGE_LAYOUT_UNDEFINED )
				{
					cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
						, ashes::getStageMask( view.layout )
						, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
							, view.viewId.data->info.subresourceRange
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, view.layout
							, VK_QUEUE_FAMILY_IGNORED
							, VK_QUEUE_FAMILY_IGNORED ) );
				}
			}
			else if ( view.layout != VK_IMAGE_LAYOUT_GENERAL
				&& view.layout != VK_IMAGE_LAYOUT_UNDEFINED )
			{
				cmd.memoryBarrier( VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, ashes::getStageMask( view.layout )
					, makeLayoutTransition( resources.createImage( context, view.viewId.data->image )
						, view.viewId.data->info.subresourceRange
						, VK_IMAGE_LAYOUT_GENERAL
						, view.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.endDebugBlock();
			cmd.end();

			return result;
		}

		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, voxelColour{ getMember< sdw::Vec4 >( "voxelColour" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "T3DTo3DSurface"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "voxelColour"
						, sdw::type::Kind::eVec4F
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "C3D_T3DTo3DSurface" );

				if ( result->empty() )
				{
					result->declMember( "voxelColour"
						, sdw::type::Kind::eVec4F
						, sdw::type::NotArray );
				}

				return result;
			}

			sdw::Vec4 voxelColour;
		};

		static ShaderPtr getVertexProgramVolume( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_GRID( writer, eGridUbo );
			auto inSource( writer.declStorageImg< RWFImg3DRgba32 >( "inSource"
				, eSource
				, 0u ) );

			shader::Utils utils{ writer };

			writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
				, VertexOutT< SurfaceT > out )
				{
					auto coord = writer.declLocale( "coord"
						, utils.unflatten( writer.cast< UInt >( in.vertexIndex )
							, uvec3( gridSize ) ) );
					out.vtx.position = vec4( vec3( coord ), 1.0f );

					out.voxelColour = inSource.load( ivec3( coord ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getGeometryProgramVolume()
		{
			using namespace sdw;
			GeometryWriter writer;

			// Shader inputs
			UBO_GRID( writer, eGridUbo );
			C3D_Camera( writer, eCameraUbo, 0u );

			// Creates a unit cube triangle strip from just vertex ID (14 vertices)
			auto createCube = writer.implementFunction< Vec3 >( "createCube"
				, [&]( UInt const vertexID )
				{
					auto b = writer.declLocale( "b"
						, 1_u << vertexID );
					writer.returnStmt( vec3( writer.ternary( ( 0x287a_u & b ) != 0u, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x02af_u & b ) != 0u, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x31e3_u & b ) != 0u, 1.0_f, 0.0_f ) ) );
				}
				, InUInt{ writer, "vertexID" } );

			writer.implementMainT< 14u, PointListT< SurfaceT >, TriangleStreamT< SurfaceT > >( [&]( GeometryIn in
				, PointListT< SurfaceT > list
				, TriangleStreamT< SurfaceT > out )
				{
					IF( writer, list[0].voxelColour.a() > 0.0f )
					{
						FOR( writer, UInt, i, 0_u, i < 14_u, ++i )
						{
							// [0, 1] => [0, 2] (y => [-1, 1])
							auto cubeVtxPos = writer.declLocale( "cubeVtxPos"
								, ( createCube( i ) - vec3( 0.0_f, 1.0f, 0.0f ) ) * 2.0f );

							// [0, gridSize]
							auto pos = writer.declLocale( "pos"
								, list[0].vtx.position.xyz() );
							// [0, gridSize] => [0, 1] => [-1, 1]
							pos = pos / writer.cast< Float >( gridSize ) * 2.0f - 1.0f;
							pos.y() = -pos.y();
							// [-1, 1] => [-gridSize, gridSize]
							pos *= writer.cast< Float >( gridSize );
							// Offset by cube position
							pos += cubeVtxPos;
							pos *= ( writer.cast< Float >( gridSize ) * ( 1.0_f / cellSize ) ) / writer.cast< Float >( gridSize );

							out.voxelColour = list[0].voxelColour;
							out.vtx.position = c3d_cameraData.worldToCurProj( vec4( pos, 1.0f ) );

							out.append();
						}
						ROF;

						out.restartStrip();
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPixelProgramVolume()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader outputs
			auto outColour = writer.declOutput< Vec4 >( "outColour", 0u );

			writer.implementMainT< SurfaceT, VoidT >( [&]( FragmentInT< SurfaceT > in
				, FragmentOut out )
				{
					IF( writer, in.voxelColour.a() > 0.0_f )
					{
						outColour = in.voxelColour;
					}
					ELSE
					{
						writer.demote();
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getVertexProgramSlice( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader outputs
			auto outUV = writer.declOutput< sdw::Vec2 >( "outUV", 0u );

			shader::Utils utils{ writer };

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					outUV = vec2( ( in.vertexIndex << 1 ) & 2, in.vertexIndex & 2 );
					out.vtx.position = vec4( outUV * 2.0f - 1.0f, 0.0f, 1.0f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPixelProgramSlice()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_GRID( writer, eGridUbo );
			auto inSource( writer.declCombinedImg< FImg3DRgba32 >( "inSource"
				, eSource
				, 0u ) );
			auto inUV = writer.declInput< sdw::Vec2 >( "inUV", 0u );

			// Shader outputs
			auto outFragColor = writer.declOutput< Vec4 >( "outFragColor", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					outFragColor = inSource.lod( vec3( inUV, sliceIndex / maxSlice ), 0.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
		, SamplerRPtr sampler )
		: descriptorSet{ t3dto2d::createDescriptorSet( device, resources, sampler, descriptorSetPool, uniformBuffer, cameraUbo, texture3D ) }
		, commands{ t3dto2d::createCommandBuffer( device, queueData, resources, renderPass, frameBuffer, pipelineLayout, pipeline, *descriptorSet, texture3D, sampler ) }
	{
	}

	Texture3DTo2D::Texture3DToScreen::Texture3DToScreen()
	{
	}

	//*********************************************************************************************

	Texture3DTo2D::Texture3DTo2D( RenderDevice const & device
		, crg::ResourcesCache & resources
		, VkExtent2D const & size
		, CameraUbo const & cameraUbo )
		: m_device{ device }
		, m_resources{ resources }
		, m_cameraUbo{ cameraUbo }
		, m_target{ t3dto2d::createTarget( device, resources, size ) }
		, m_depthBuffer{ t3dto2d::createDepthBuffer( device, resources, m_target ) }
		, m_uniformBuffer{ device.uboPool->getBuffer< Texture3DTo2DData >( 0u ) }
		, m_renderPass{ t3dto2d::createRenderPass( device, "Texture3DTo2D", m_target, m_depthBuffer ) }
		, m_frameBuffer{ t3dto2d::createFramebuffer( *m_renderPass, "Texture3DTo2D", m_target, m_depthBuffer ) }
		, m_sampler{ castor::makeUnique< Sampler >( "Slice"
			, *device.renderSystem.getEngine()
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

		, m_descriptorSetLayoutVolume{ t3dto2d::createDescriptorLayout( device, false ) }
		, m_pipelineLayoutVolume{ t3dto2d::createPipelineLayout( device, *m_descriptorSetLayoutVolume ) }
		, m_vertexShaderVolume{ VK_SHADER_STAGE_VERTEX_BIT, "Texture3DTo2D", t3dto2d::getVertexProgramVolume( device.renderSystem ) }
		, m_geometryShaderVolume{ VK_SHADER_STAGE_GEOMETRY_BIT, "Texture3DTo2D", t3dto2d::getGeometryProgramVolume() }
		, m_pixelShaderVolume{ VK_SHADER_STAGE_FRAGMENT_BIT, "Texture3DTo2D", t3dto2d::getPixelProgramVolume() }
		, m_pipelineVolume{ t3dto2d::createPipeline( device, *m_pipelineLayoutVolume, *m_renderPass, m_vertexShaderVolume, m_geometryShaderVolume, m_pixelShaderVolume, m_target ) }

		, m_descriptorSetLayoutSlice{ t3dto2d::createDescriptorLayout( device, true ) }
		, m_pipelineLayoutSlice{ t3dto2d::createPipelineLayout( device, *m_descriptorSetLayoutSlice ) }
		, m_vertexShaderSlice{ VK_SHADER_STAGE_VERTEX_BIT, "Texture3DTo2D", t3dto2d::getVertexProgramSlice( device.renderSystem ) }
		, m_pixelShaderSlice{ VK_SHADER_STAGE_FRAGMENT_BIT, "Texture3DTo2D", t3dto2d::getPixelProgramSlice() }
		, m_pipelineSlice{ t3dto2d::createPipeline( device, *m_pipelineLayoutSlice, *m_renderPass, m_vertexShaderSlice, m_pixelShaderSlice, m_target ) }
	{
		m_sampler->initialise( device );
	}

	Texture3DTo2D::~Texture3DTo2D()
	{
		m_sampler->cleanup();
		m_depthBuffer.destroy();
		m_target.destroy();
		m_device.uboPool->putBuffer( m_uniformBuffer );
	}

	void Texture3DTo2D::createPasses( QueueData const & queueData
		, IntermediateViewArray intermediates )
	{
		m_textures = std::move( intermediates );
		initialise( queueData );
	}

	void Texture3DTo2D::initialise( QueueData const & queueData )
	{
		m_descriptorSetPoolVolume = m_descriptorSetLayoutVolume->createPool( uint32_t( m_textures.size() ) );
		m_descriptorSetPoolSlice = m_descriptorSetLayoutSlice->createPool( uint32_t( m_textures.size() ) );

		for ( auto & intermediate : m_textures )
		{
			if ( intermediate.viewId.data->image.data->info.imageType == VK_IMAGE_TYPE_3D )
			{
				if ( intermediate.factors.isSlice )
				{
					m_texture3DToScreen.emplace_back( m_device
						, queueData
						, m_resources
						, m_uniformBuffer
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
					m_texture3DToScreen.emplace_back( m_device
						, queueData
						, m_resources
						, m_uniformBuffer
						, m_cameraUbo
						, intermediate
						, *m_renderPass
						, *m_descriptorSetPoolVolume
						, *m_frameBuffer
						, *m_pipelineLayoutVolume
						, *m_pipelineVolume
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
			auto & data = m_uniformBuffer.getData();
			data.gridCenterCellSize = castor::Point4f{ 0.0f
				, 0.0f
				, m_textures[m_index].factors.slice
				, m_textures[m_index].viewId.data->image.data->info.extent.depth - 1u };
		}
		else if ( updater.cellSize != 0.0f )
		{
			auto & data = m_uniformBuffer.getData();
			data.gridCenterCellSize = castor::Point4f{ updater.gridCenter->x
				, updater.gridCenter->y
				, updater.gridCenter->z
				, updater.cellSize };
			data.gridSize = getExtent( m_textures[m_index].viewId ).width;
		}
	}

	crg::SemaphoreWait Texture3DTo2D::render( ashes::Queue const & queue
		, crg::SemaphoreWait const & toWait )
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
