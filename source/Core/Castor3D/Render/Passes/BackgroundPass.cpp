#include "Castor3D/Render/Passes/BackgroundPass.hpp"

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/GraphContext.hpp>

#include <ashes/ashes.hpp>

namespace castor3d
{
	BackgroundPass::BackgroundPass( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, SceneBackground & background
		, VkExtent2D const & size
		, bool usesDepth )
		: crg::RenderPass{ pass, context, graph }
		, m_device{ device }
		, m_background{ background }
		, m_size{ size }
		, m_usesDepth{ usesDepth }
		, m_onBackgroundChanged{ background.onChanged.connect( [this]( SceneBackground const & )
			{
				//getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				//	, [this]( RenderDevice const & device )
				//	{
				//		doPrepareBgCommands( m_device
				//			, *m_renderTarget.getScene()->getBackground()
				//			, *m_bgRenderPass
				//			, *m_bgFrameBuffer
				//			, m_renderTarget.getHdrConfigUbo()
				//			, *m_bgCommandBuffer );
				//	} ) );
			} ) }
	{
	}

	BackgroundPass::~BackgroundPass()
	{
		m_onBackgroundChanged.disconnect();
	}

	void BackgroundPass::update( CpuUpdater & updater )
	{
		m_background.update( updater );
	}

	void BackgroundPass::update( GpuUpdater & updater )
	{
		m_background.update( updater );
	}

	void BackgroundPass::doSubInitialise()
	{
		m_background.initialise( m_device );
		doInitialiseVertexBuffer();
		doFillDescriptorBindings();
		m_descriptorSetLayout = m_device->createDescriptorSetLayout( m_pass.name
			, m_descriptorBindings );
		m_pipelineLayout = m_device->createPipelineLayout( m_pass.name
			, *m_descriptorSetLayout );
		m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
		doCreateDescriptorSet();
		doCreatePipeline();
	}

	void BackgroundPass::doSubRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkDeviceSize offset{};
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		VkBuffer vbo = m_vertexBuffer->getBuffer();
		m_context.vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipelineLayout, 0u, 1u, &descriptorSet, 0u, nullptr );
		m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vbo, &offset );
		m_context.vkCmdBindIndexBuffer( commandBuffer, m_indexBuffer->getBuffer(), 0u, VK_INDEX_TYPE_UINT16 );
		m_context.vkCmdDrawIndexed( commandBuffer, uint32_t( m_indexBuffer->getCount() ), 1u, 0u, 0u, 0u );
	}

	void BackgroundPass::doInitialiseVertexBuffer()
	{
		using castor::Point3f;
		ashes::StagingBuffer stagingBuffer{ *m_device.device, 0u, sizeof( Cube ) };

		// Vertex Buffer
		std::vector< Cube > vertexData
		{
			Cube
			{
				{
					// Front
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 } },
					// Top
					{ Point3f{ -1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					// Back
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 } },
					// Bottom
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, +1.0 } },
					// Right
					{ Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					// Left
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
				}
			}
		};
		m_vertexBuffer = makeVertexBuffer< Cube >( m_device
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Background");
		stagingBuffer.uploadVertexData( *m_device.graphicsQueue
			, *m_device.graphicsCommandPool
			, vertexData
			, *m_vertexBuffer );

		// Index Buffer
		std::vector< uint16_t > indexData
		{
			// Front
			0, 1, 2, 2, 1, 3,
			// Top
			4, 5, 6, 6, 5, 7,
			// Back
			8, 9, 10, 10, 9, 11,
			// Bottom
			12, 13, 14, 14, 13, 15,
			// Right
			16, 17, 18, 18, 17, 19,
			// Left
			20, 21, 22, 22, 21, 23,
		};
		m_indexBuffer = makeBuffer< uint16_t >( m_device
			, uint32_t( indexData.size() )
			, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "BackgroundIndexBuffer" );
		stagingBuffer.uploadBufferData( *m_device.graphicsQueue
			, *m_device.graphicsCommandPool
			, indexData
			, *m_indexBuffer );
	}

	ashes::PipelineShaderStageCreateInfoArray BackgroundPass::doInitialiseShader()
	{
		using namespace sdw;

		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Background" };
		{
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec3 >( "position", 0u );
			UBO_MATRIX( writer, SceneBackground::MtxUboIdx, 0u );
			UBO_MODEL( writer, SceneBackground::MdlMtxUboIdx, 0u );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			std::function< void() > main = [&]()
			{
				out.vtx.position = c3d_matrixData.worldToCurProj( c3d_modelData.modelToWorld( vec4( position, 1.0_f ) ) ).xyww();
				vtx_texture = position;
			};

			writer.implementFunction< sdw::Void >( "main", main );
			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Background" };
		{
			FragmentWriter writer;

			// Inputs
			UBO_HDR_CONFIG( writer, SceneBackground::HdrCfgUboIdx, 0u );
			auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture", 0u );
			auto c3d_mapSkybox = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapSkybox", SceneBackground::SkyBoxImgIdx, 0u );
			shader::Utils utils{ writer };

			if ( !m_background.isHdr() )
			{
				utils.declareRemoveGamma();
			}

			// Outputs
			auto pxl_FragColor = writer.declOutput< Vec4 >( "pxl_FragColor", 0u );

			std::function< void() > main = [&]()
			{
				auto colour = writer.declLocale( "colour"
					, c3d_mapSkybox.sample( vtx_texture ) );

				if ( !m_background.isHdr() )
				{
					pxl_FragColor = vec4( c3d_hdrConfigData.removeGamma( colour.xyz() ), colour.w() );
				}
				else
				{
					pxl_FragColor = vec4( colour.xyz(), colour.w() );
				}
			};

			writer.implementFunction< sdw::Void >( "main", main );
			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( m_device, vtx ),
			makeShaderState( m_device, pxl ),
		};
	}

	void BackgroundPass::doFillDescriptorBindings()
	{
		VkShaderStageFlags shaderStage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		for ( auto & uniform : m_pass.buffers )
		{
			m_descriptorBindings.push_back( { uniform.binding
				, uniform.getDescriptorType()
				, 1u
				, shaderStage
				, nullptr } );
			m_descriptorWrites.push_back( uniform.getBufferWrite() );
		}

		m_descriptorBindings.push_back( { SceneBackground::SkyBoxImgIdx
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, shaderStage
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ SceneBackground::SkyBoxImgIdx
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background.getSampler().getSampler()
				, m_background.getTexture().getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
	}

	void BackgroundPass::doCreateDescriptorSet()
	{
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet( m_pass.name );

		for ( auto & write : m_descriptorWrites )
		{
			write.update( *m_descriptorSet );
		}

		m_descriptorSet->setBindings( crg::makeVkArray< VkWriteDescriptorSet >( m_descriptorWrites ) );
		m_descriptorSet->update();
	}

	void BackgroundPass::doCreatePipeline()
	{
		m_pipeline = m_device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
			, doInitialiseShader()
			, ashes::PipelineVertexInputStateCreateInfo{ 0u
				, { 1u, { 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } }
				, { 1u, { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } }
			, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE }
			, std::nullopt
			, doCreateViewportState()
			, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
			, ashes::PipelineMultisampleStateCreateInfo{}
			, ( m_usesDepth
				? std::make_optional( ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL } )
				: std::nullopt )
			, ashes::PipelineColorBlendStateCreateInfo{ 0u, VK_FALSE, VK_LOGIC_OP_COPY, m_blendAttachs }
			, std::nullopt
			, *m_pipelineLayout
			, m_renderPass } );
	}

	ashes::PipelineViewportStateCreateInfo BackgroundPass::doCreateViewportState()
	{
		ashes::VkViewportArray viewports( 1u
			, { float( 0.0f )
				, float( 0.0f )
				, float( m_size.width )
				, float( m_size.height ) } );
		ashes::VkScissorArray scissors( 1u
			, { 0
				, 0
				, m_size.width
				, m_size.height } );
		return { 0u
			, uint32_t( viewports.size())
			, viewports
			, uint32_t( scissors.size())
			, scissors };
	}
}
