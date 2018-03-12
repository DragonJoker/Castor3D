#include "EquirectangularToCube.hpp"

#include "Render/RenderSystem.hpp"

#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <GlslSource.hpp>

#include <Math/TransformationMatrix.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace
	{
		renderer::SamplerPtr doCreateSampler( renderer::Device const & device )
		{
			return device.createSampler( renderer::WrapMode::eClampToEdge
				, renderer::WrapMode::eClampToEdge
				, renderer::WrapMode::eClampToEdge
				, renderer::Filter::eLinear
				, renderer::Filter::eLinear );
		}

		renderer::UniformBufferPtr< castor::Matrix4x4f > doCreateMatrixUbo( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer )
		{
			static Matrix4x4f const projection = convert( device.perspective( float( 90.0_degrees ), 1.0f, 0.1f, 10.0f ) );
			static Matrix4x4f const views[] =
			{
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ -1.0f, +0.0f, +0.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, +1.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, -1.0f, +0.0f }, Point3f{ 0.0f, +0.0f, -1.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, +1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } ),
				matrix::lookAt( Point3f{ 0.0f, 0.0f, 0.0f }, Point3f{ +0.0f, +0.0f, -1.0f }, Point3f{ 0.0f, -1.0f, +0.0f } )
			};

			auto result = renderer::makeUniformBuffer< castor::Matrix4x4f >( device
				, 6u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( device.getClipDirection() == renderer::ClipDirection::eTopDown )
			{
				result->getData( 0u ) = projection * views[0];
				result->getData( 1u ) = projection * views[1];
				result->getData( 2u ) = projection * views[2];
				result->getData( 3u ) = projection * views[3];
				result->getData( 4u ) = projection * views[4];
				result->getData( 5u ) = projection * views[5];
			}
			else
			{
				result->getData( 0u ) = projection * views[0];
				result->getData( 1u ) = projection * views[1];
				result->getData( 2u ) = projection * views[3];
				result->getData( 3u ) = projection * views[2];
				result->getData( 4u ) = projection * views[4];
				result->getData( 5u ) = projection * views[5];
			}

			result->upload();
			return result;
		}

		renderer::ShaderStageStateArray doCreateProgram( RenderSystem & renderSystem )
		{
			auto & device = *renderSystem.getCurrentDevice();

			glsl::Shader vtx;
			{
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto position = writer.declAttribute< Vec4 >( cuT( "position" ), 0u );
				auto matrixUbo = glsl::Ubo{ writer, cuT( "Matrix" ), 0u, 0u };
				auto mtxViewProjection = matrixUbo.declMember< Mat4 >( cuT( "mtxViewProjection" ) );
				matrixUbo.end();

				// Outputs
				auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ), 0u );
				auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

				std::function< void() > main = [&]()
				{
					vtx_position = position.xyz();
					gl_Position = writer.paren( mtxViewProjection * vec4( position ) );
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				vtx = writer.finalise();
			}

			glsl::Shader pxl;
			{
				GlslWriter writer{ renderSystem.createGlslWriter() };

				// Inputs
				auto mapColour = writer.declSampler< Sampler2D >( cuT( "mapColour" ), 0u, 0u );
				auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ), 0u );

				// Outputs
				auto pxl_colour = writer.declFragData< Vec4 >( cuT( "pxl_colour" ), 0u );

				auto sampleSphericalMap = writer.implementFunction< Vec2 >( cuT( "sampleSphericalMap" )
					, [&]( Vec3 const & v )
				{
					auto uv = writer.declLocale( cuT( "uv" ), vec2( atan( v.z(), v.x() ), asin( v.y() ) ) );
					uv *= vec2( 0.1591_f, 0.3183_f );
					uv += 0.5_f;
					writer.returnStmt( uv );
				}
				, InVec3{ &writer, cuT( "v" ) } );

				std::function< void() > main = [&]()
				{
					auto uv = writer.declLocale( cuT( "uv" ), sampleSphericalMap( normalize( vtx_position ) ) );

					if ( device.getClipDirection() == renderer::ClipDirection::eTopDown )
					{
						pxl_colour = vec4( texture( mapColour, vec2( uv.x(), 1.0 - uv.y() ) ).rgb(), 1.0 );
					}
					else
					{
						pxl_colour = vec4( texture( mapColour, vec2( uv.x(), uv.y() ) ).rgb(), 1.0 );
					}
				};

				writer.implementFunction< void >( cuT( "main" ), main );
				pxl = writer.finalise();
			}

			std::vector< renderer::ShaderStageState > shaderStages;
			shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
			shaderStages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
			shaderStages[0].module->loadShader( vtx.getSource() );
			shaderStages[1].module->loadShader( pxl.getSource() );

			return shaderStages;
		}

		renderer::VertexBufferPtr< Point4f > doCreateVertexBuffer( renderer::Device const & device
			, renderer::CommandBuffer const & commandBuffer )
		{
			std::vector< Point4f > vertexData
			{
				Point4f{ -1, +1, -1, 1 }, Point4f{ +1, -1, -1, 1 }, Point4f{ -1, -1, -1, 1 }, Point4f{ +1, -1, -1, 1 }, Point4f{ -1, +1, -1, 1 }, Point4f{ +1, +1, -1, 1 },// Back
				Point4f{ -1, -1, +1, 1 }, Point4f{ -1, +1, -1, 1 }, Point4f{ -1, -1, -1, 1 }, Point4f{ -1, +1, -1, 1 }, Point4f{ -1, -1, +1, 1 }, Point4f{ -1, +1, +1, 1 },// Left
				Point4f{ +1, -1, -1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ +1, -1, +1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ +1, -1, -1, 1 }, Point4f{ +1, +1, -1, 1 },// Right
				Point4f{ -1, -1, +1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ -1, +1, +1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ -1, -1, +1, 1 }, Point4f{ +1, -1, +1, 1 },// Front
				Point4f{ -1, +1, -1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ +1, +1, -1, 1 }, Point4f{ +1, +1, +1, 1 }, Point4f{ -1, +1, -1, 1 }, Point4f{ -1, +1, +1, 1 },// Top
				Point4f{ -1, -1, -1, 1 }, Point4f{ +1, -1, -1, 1 }, Point4f{ -1, -1, +1, 1 }, Point4f{ +1, -1, -1, 1 }, Point4f{ +1, -1, +1, 1 }, Point4f{ -1, -1, +1, 1 },// Bottom
			};
			auto result = renderer::makeVertexBuffer< Point4f >( device
				, uint32_t( vertexData.size() )
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto * buffer = result->lock( 0u, result->getCount(), renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, vertexData.data(), vertexData.size() );
				result->flush( 0u, result->getCount() );
				result->unlock();
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( renderer::Device const & device )
		{
			auto result = renderer::makeLayout< Point4f >( 0u );
			result->createAttribute( 0u, renderer::Format::eR32G32B32A32_SFLOAT, 0u );
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorSetLayout( renderer::Device const & device )
		{
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}
	}

	EquirectangularToCube::EquirectangularToCube( renderer::Texture const & equiRectangular
		, RenderSystem & renderSystem
		, renderer::Texture const & texture )
		: m_device{ *renderSystem.getCurrentDevice() }
		, m_target{ texture }
		, m_commandBuffer{ m_device.getGraphicsCommandPool().createCommandBuffer() }
		, m_view{ m_texture->createView( renderer::TextureViewType::e2D, equiRectangular.getFormat() ) }
		, m_sampler{ doCreateSampler( m_device ) }
		, m_matrixUbo{ doCreateMatrixUbo( m_device, *m_commandBuffer ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_device, *m_commandBuffer ) }
		, m_vertexLayout{ doCreateVertexLayout( m_device ) }
		, m_descriptorLayout{ doCreateDescriptorSetLayout( m_device ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( 6u ) }
		, m_pipelineLayout{ m_device.createPipelineLayout( *m_descriptorLayout ) }
	{
		auto size = renderer::Extent2D{ texture.getDimensions().width, texture.getDimensions().height };
		uint32_t face = 0u;
		std::vector< renderer::Format > formats{ 1u, m_target.getFormat() };
		renderer::AttachmentDescriptionArray rpAttaches
		{
			{
				0u,
				m_target.getFormat(),
				renderer::SampleCountFlag::e1,
				renderer::AttachmentLoadOp::eClear,
				renderer::AttachmentStoreOp::eStore,
				renderer::AttachmentLoadOp::eDontCare,
				renderer::AttachmentStoreOp::eDontCare,
				renderer::ImageLayout::eUndefined,
				renderer::ImageLayout::eShaderReadOnlyOptimal,
			}
		};
		renderer::AttachmentReferenceArray subAttaches
		{
			{ 0u, renderer::ImageLayout::eColourAttachmentOptimal }
		};

		for ( auto & facePipeline : m_faces )
		{
			facePipeline.view = m_target.createView( renderer::TextureViewType::e2D
				, m_target.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			++face;
		}

		face = 0u;
		auto program = doCreateProgram( renderSystem );

		for ( auto & facePipeline : m_faces )
		{
			renderer::RenderSubpassPtrArray subpasses;
			subpasses.emplace_back( std::make_unique< renderer::RenderSubpass >( renderer::PipelineBindPoint::eGraphics
				, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite }
				, subAttaches ) );
			facePipeline.renderPass = m_device.createRenderPass( rpAttaches
				, std::move( subpasses )
				, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite }
				, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eShaderRead } );

			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *facePipeline.renderPass->getAttachments().begin(), *facePipeline.view );
			facePipeline.frameBuffer = facePipeline.renderPass->createFrameBuffer( size
				, std::move( attaches ) );

			facePipeline.pipeline = m_pipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
			{
				program,
				*facePipeline.renderPass,
				renderer::VertexInputState::create( *m_vertexLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{},
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false },
				renderer::TessellationState{},
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height }
			} );

			facePipeline.descriptorSet = m_descriptorPool->createDescriptorSet();
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
				, *m_matrixUbo
				, face
				, 1u );
			facePipeline.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
				, *m_view
				, *m_sampler );
			facePipeline.descriptorSet->update();
			++face;
		}
	}

	void EquirectangularToCube::render( renderer::CommandBuffer & commandBuffer )
	{
		for ( auto & facePipeline : m_faces )
		{
			commandBuffer.memoryBarrier( renderer::PipelineStageFlag::eTransfer
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, facePipeline.view->makeColourAttachment( renderer::ImageLayout::eUndefined
					, 0u ) );
			commandBuffer.beginRenderPass( *facePipeline.renderPass
				, *facePipeline.frameBuffer
				, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
			, renderer::SubpassContents::eInline );
			commandBuffer.bindPipeline( *facePipeline.pipeline );
			commandBuffer.bindDescriptorSet( *facePipeline.descriptorSet
				, *m_pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			commandBuffer.draw( 36u );
			commandBuffer.endRenderPass();
		}

	}

	void EquirectangularToCube::render()
	{
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			render( *m_commandBuffer );
			m_commandBuffer->end();
			m_device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
			m_device.getGraphicsQueue().waitIdle();
		}
	}
}
