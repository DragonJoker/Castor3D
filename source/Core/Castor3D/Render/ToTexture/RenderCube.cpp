#include "Castor3D/Render/ToTexture/RenderCube.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/DirectUploadData.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/InstantUploadData.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

CU_ImplementSmartPtr( c3d, RenderCube )

namespace c3d
{
	namespace rendcube
	{
		static uint32_t constexpr MtxUboIdx = 0u;
		static uint32_t constexpr InputImgIdx = 1u;

		static SamplerObs doCreateSampler( RenderSystem const & renderSystem
			, bool nearest )
		{
			String const name = nearest
				? String{ cuT( "RenderCube_Nearest" ) }
				: String{ cuT( "RenderCube_Linear" ) };
			FilterMode const minMagFilter = nearest
				? FilterMode::eNearest
				: FilterMode::eLinear;
			MipmapMode const mipFilter = nearest
				? MipmapMode::eNearest
				: MipmapMode::eLinear;
			auto & engine = *renderSystem.getEngine();
			SamplerObs result{};

			if ( engine.hasSampler( name ) )
			{
				result = engine.findSampler( name );
			}
			else if ( auto sampler = engine.addNewSampler( name, engine ) )
			{	
				sampler->setMinFilter( minMagFilter );
				sampler->setMagFilter( minMagFilter );
				sampler->setMipFilter( mipFilter );
				sampler->setWrapS( WrapMode::eClampToEdge );
				sampler->setWrapT( WrapMode::eClampToEdge );
				sampler->setWrapR( WrapMode::eClampToEdge );
				result = sampler;
			}

			return result;
		}

		static UniformBufferOffsetT< Matrix4x4f > doCreateMatrixUbo( RenderDevice const & device, uint32_t face )
		{
			static Matrix4x4f const projection = device.renderSystem.getPerspective( 90.0_degrees, 1.0f, 0.1f, 10.0f );

			static Array< Matrix4x4f, 6u > const views = []()
			{
				Array< Matrix4x4f, 6u > result
				{
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, +1.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, -1.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
					matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } )
				};
				return result;
			}();
			auto result = device.uboPool->getBuffer< Matrix4x4f >( MemoryPropertyFlags::eHostVisible );
				result.getData() = projection * views[face];
			return result;
		}

		static GpuBufferOffsetT< Point4f > doCreateVertexBuffer( RenderDevice const & device )
		{
			Vector< Point4f > vertexData
			{
				Point4f{ -1, +1, -1, +1 }, Point4f{ +1, -1, -1, +1 }, Point4f{ -1, -1, -1, +1 }, Point4f{ +1, -1, -1, +1 }, Point4f{ -1, +1, -1, +1 }, Point4f{ +1, +1, -1, +1 },// Back
				Point4f{ -1, -1, +1, +1 }, Point4f{ -1, +1, -1, +1 }, Point4f{ -1, -1, -1, +1 }, Point4f{ -1, +1, -1, +1 }, Point4f{ -1, -1, +1, +1 }, Point4f{ -1, +1, +1, +1 },// Left
				Point4f{ +1, -1, -1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ +1, -1, +1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ +1, -1, -1, +1 }, Point4f{ +1, +1, -1, +1 },// Right
				Point4f{ -1, -1, +1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ -1, +1, +1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ -1, -1, +1, +1 }, Point4f{ +1, -1, +1, +1 },// Front
				Point4f{ -1, +1, -1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ +1, +1, -1, +1 }, Point4f{ +1, +1, +1, +1 }, Point4f{ -1, +1, -1, +1 }, Point4f{ -1, +1, +1, +1 },// Top
				Point4f{ -1, -1, -1, +1 }, Point4f{ +1, -1, -1, +1 }, Point4f{ -1, -1, +1, +1 }, Point4f{ +1, -1, -1, +1 }, Point4f{ +1, -1, +1, +1 }, Point4f{ -1, -1, +1, +1 },// Bottom
			};
			auto result = device.bufferPool->getBuffer< Point4f >( BufferUsageFlags::eTransferDst | BufferUsageFlags::eVertexBuffer
				, uint32_t( vertexData.size() )
				, MemoryPropertyFlags::eHostVisible );
			for ( uint32_t i = 0u; i < vertexData.size(); ++i )
				result.getData()[i] = vertexData[i];

			{
				InstantDirectUploadData uploader{ *device.transferQueue
					, device, cuT( "RenderCubeVBUpload" ), *device.transferCommandPool };
				result.upload( uploader, VertexAttributeInputState );
			}

			return result;
		}

		static ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout()
		{
			return ashes::PipelineVertexInputStateCreateInfo
			{
				0u,
				{
					{ 0u, uint32_t( sizeof( Point4f ) ), VK_VERTEX_INPUT_RATE_VERTEX },
				},
				{
					{ 0u, 0u, VK_FORMAT_R32G32B32A32_SFLOAT, 0u }
				},
			};
		}
	}

	RenderCube::RenderCube( RenderDevice const & device
		, bool nearest
		, SamplerObs sampler )
		: m_device{ device }
		, m_sampler{ ( sampler
			? c3d::move( sampler )
			: rendcube::doCreateSampler( m_device.renderSystem, nearest ) ) }
	{
	}

	RenderCube::~RenderCube()
	{
		cleanup();
	}

	void RenderCube::createPipelines( Extent2D const & size
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkPushConstantRangeArray const & pushRanges )
	{
		createPipelines( size
			, program
			, view
			, renderPass
			, pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
	}

	void RenderCube::createPipelines( Extent2D const & size
		, ashes::PipelineShaderStageCreateInfoArray const & program
		, ashes::ImageView const & view
		, ashes::RenderPass const & renderPass
		, ashes::VkPushConstantRangeArray const & pushRanges
		, ashes::PipelineDepthStencilStateCreateInfo const & dsState )
	{
		auto queueData = m_device.graphicsData();
		m_sampler->initialise( m_device );
		m_vertexBuffer = rendcube::doCreateVertexBuffer( m_device );
		auto vertexLayout = rendcube::doCreateVertexLayout();

		// Initialise the descriptor set.
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( rendcube::MtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( rendcube::InputImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		doFillDescriptorLayoutBindings( bindings );
		m_descriptorLayout = m_device->createDescriptorSetLayout( "RenderCube"
			, c3d::move( bindings ) );
		m_pipelineLayout = m_device->createPipelineLayout( "RenderCube"
			, { *m_descriptorLayout }, pushRanges );
		m_descriptorPool = m_descriptorLayout->createPool( "RenderCube", 6u );
		uint32_t face = 0u;

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.matrixUbo = rendcube::doCreateMatrixUbo( m_device, face );
			facePipeline.pipeline = m_device->createPipeline( "RenderCubeFace" + string::toMbString( face )
				, ashes::GraphicsPipelineCreateInfo
				{
					0u,
					program,
					vertexLayout,
					ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
					ashes::nullopt,
					ashes::PipelineViewportStateCreateInfo{ 0u, 1u, { VkViewport{ 0.0f, 0.0f, float( size.width ), float( size.height ), 0.0f, 1.0f } }, 1u, { VkRect2D{ 0, 0, size.width, size.height } } },
					ashes::PipelineRasterizationStateCreateInfo{ 0u, false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
					ashes::PipelineMultisampleStateCreateInfo{},
					dsState,
					ashes::PipelineColorBlendStateCreateInfo{},
					ashes::nullopt,
					*m_pipelineLayout,
					renderPass,
				} );
			facePipeline.descriptorSet = m_descriptorPool->createDescriptorSet( "RenderCubeFace" + string::toMbString( face ) );
			ashes::WriteDescriptorSetArray writes;
			facePipeline.matrixUbo.addDescriptorWriteT( writes, 0u );
			writes.push_back( makeImageViewDescriptorWrite( view, m_sampler->getSampler(), 1u ) );
			doFillDescriptorWrites( writes, face );
			facePipeline.descriptorSet->setBindings( c3d::move( writes ) );
			facePipeline.descriptorSet->update();
			++face;
		}
	}

	void RenderCube::cleanup()noexcept
	{
		m_commandBuffer.reset();

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.descriptorSet.reset();
			facePipeline.pipeline.reset();
			m_device.uboPool->putBuffer( facePipeline.matrixUbo );
		}

		m_descriptorPool.reset();
		m_pipelineLayout.reset();
		m_descriptorLayout.reset();
		m_device.bufferPool->putBuffer( m_vertexBuffer );
	}

	void RenderCube::prepareFrame( ashes::RenderPass const & renderPass
		, uint32_t subpassIndex
		, uint32_t face )
	{
		m_commandBuffer = m_device.graphicsData()->commandPool->createCommandBuffer( "RenderCube"
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, subpassIndex
				, VK_NULL_HANDLE
				, VK_FALSE
				, 0u
				, 0u ) );
		registerFrame( *m_commandBuffer, face );
		m_commandBuffer->end();
	}

	void RenderCube::registerFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
		auto & facePipeline = m_faces[face];
		commandBuffer.bindPipeline( *facePipeline.pipeline );
		commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet, *m_pipelineLayout );
		commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer.getBuffer().getBuffer(), m_vertexBuffer.getOffset() );
		doRegisterFrame( commandBuffer, face );
		commandBuffer.draw( 36u );
	}

	void RenderCube::doFillDescriptorLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )
	{
	}

	void RenderCube::doFillDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t face )
	{
	}

	void RenderCube::doRegisterFrame( ashes::CommandBuffer & commandBuffer
		, uint32_t face )const
	{
	}
}
