#include "RenderPanel.hpp"
#include "Application.hpp"

#include <Core/BackBuffer.hpp>
#include <Sync/BufferMemoryBarrier.hpp>
#include <Command/CommandBuffer.hpp>
#include <Core/Connection.hpp>
#include <Core/Device.hpp>
#include <Buffer/GeometryBuffers.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Core/Renderer.hpp>
#include <Core/RenderingResources.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Enum/SubpassContents.hpp>
#include <Core/SwapChain.hpp>
#include <Image/Texture.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>

#include <FileUtils.hpp>

#include <fstream>
#include <cstring>

namespace vkapp
{
	namespace
	{
		enum class Ids
		{
			RenderTimer = 42
		}	Ids;

		static int const TimerTimeMs = 40;
	}

	RenderPanel::RenderPanel( wxWindow * parent
		, wxSize const & size
		, renderer::Renderer const & renderer )
		: wxPanel{ parent, wxID_ANY, wxDefaultPosition, size }
		, m_timer{ new wxTimer{ this, int( Ids::RenderTimer ) } }
		, m_vertexData{
		{
			{
				{ -0.7f, -0.7f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 0.0f }
			},
			{
				{ -0.7f, 0.7f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f }
			},
			{
				{ 0.7f, -0.7f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f }
			},
			{
				{ 0.7f, 0.7f, 0.0f, 1.0f },
				{ 0.3f, 0.3f, 0.3f, 0.0f }
			}
		} }
	{
		try
		{
			doCreateDevice( renderer );
			std::cout << "Logical device created." << std::endl;
			doCreateSwapChain();
			std::cout << "Swapchain created." << std::endl;
			doCreateStagingBuffer();
			std::cout << "Staging buffer created." << std::endl;
			doCreateRenderPass();
			std::cout << "Render pass created." << std::endl;
			doCreateVertexBuffer();
			std::cout << "Vertex buffer created." << std::endl;
			doCreatePipeline();
			std::cout << "Pipeline created." << std::endl;
			doPrepareFrames();
			std::cout << "Frames prepared." << std::endl;
		}
		catch ( std::exception & )
		{
			doCleanup();
			throw;
		}

		m_timer->Start( TimerTimeMs );

		Connect( int( Ids::RenderTimer ), wxEVT_TIMER, wxTimerEventHandler( RenderPanel::onTimer ), nullptr, this );
		Connect( wxID_ANY, wxEVT_SIZE, wxSizeEventHandler( RenderPanel::onSize ), nullptr, this );
	}

	RenderPanel::~RenderPanel()
	{
		doCleanup();
	}

	void RenderPanel::doCleanup()
	{
		delete m_timer;

		if ( m_device )
		{
			m_device->waitIdle();
			m_device->waitIdle();
			m_stagingBuffer.reset();
			m_pipeline.reset();
			m_vertexLayout.reset();
			m_program.reset();
			m_pipelineLayout.reset();
			m_geometryBuffers.reset();
			m_vertexBuffer.reset();
			m_commandBuffers.clear();
			m_frameBuffers.clear();
			m_renderPass.reset();
			m_swapChain.reset();
			m_device->disable();
			m_device.reset();
		}
	}

	void RenderPanel::doCreateDevice( renderer::Renderer const & renderer )
	{
		m_device = renderer.createDevice( common::makeConnection( this, renderer ) );
		m_device->enable();
	}

	void RenderPanel::doCreateSwapChain()
	{
		wxSize size{ GetClientSize() };
		m_swapChain = m_device->createSwapChain( { size.x, size.y } );
		m_swapChain->setClearColour( renderer::RgbaColour{ 1.0f, 0.8f, 0.4f, 0.0f } );
		m_swapChainReset = m_swapChain->onReset.connect( [this]()
		{
			doResetSwapChain();
		} );
	}

	void RenderPanel::doCreateRenderPass()
	{
		std::vector< renderer::PixelFormat > formats{ { m_swapChain->getFormat() } };
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( m_device->createRenderSubpass( formats
			, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::AccessFlag::eColourAttachmentWrite } ) );
		m_renderPass = m_device->createRenderPass( formats
			, std::move( subpasses )
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eBottomOfPipe
			, renderer::AccessFlag::eMemoryRead
			, { renderer::ImageLayout::ePresentSrc } }
			, renderer::RenderPassState{ renderer::PipelineStageFlag::eBottomOfPipe
			, renderer::AccessFlag::eMemoryRead
			, { renderer::ImageLayout::ePresentSrc } } );
	}

	void RenderPanel::doCreateVertexBuffer()
	{
		m_vertexBuffer = renderer::makeVertexBuffer< VertexData >( *m_device
			, uint32_t( m_vertexData.size() )
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_stagingBuffer->uploadVertexData( m_swapChain->getDefaultResources().getCommandBuffer()
			, m_vertexData
			, *m_vertexBuffer
			, renderer::PipelineStageFlag::eVertexInput );

		m_vertexLayout =  renderer::makeLayout< VertexData >( *m_device, 0u );
		m_vertexLayout->createAttribute< renderer::Vec4 >( 0u
			, uint32_t( offsetof( VertexData, position ) ) );
		m_vertexLayout->createAttribute< renderer::Vec4 >( 1u
			, uint32_t( offsetof( VertexData, colour ) ) );

		m_geometryBuffers = m_device->createGeometryBuffers( *m_vertexBuffer
			, 0u
			, *m_vertexLayout );
	}

	void RenderPanel::doCreateStagingBuffer()
	{
		m_stagingBuffer = std::make_unique< renderer::StagingBuffer >( *m_device
			, 0u
			, 4000u );
	}

	void RenderPanel::doCreatePipeline()
	{
		m_pipelineLayout = m_device->createPipelineLayout();
		wxSize size{ GetClientSize() };
		std::string shadersFolder = common::getPath( common::getExecutableDirectory() ) / "share" / AppName / "Shaders";
		m_program = m_device->createShaderProgram();

		if ( m_program->isSPIRVSupported() )
		{
			if ( !wxFileExists( shadersFolder / "vert.spv" )
				|| !wxFileExists( shadersFolder / "frag.spv" ) )
			{
				throw std::runtime_error{ "Shader files are missing" };
			}

			m_program->createModule( common::dumpBinaryFile( shadersFolder / "vert.spv" )
				, renderer::ShaderStageFlag::eVertex );
			m_program->createModule( common::dumpBinaryFile( shadersFolder / "frag.spv" )
				, renderer::ShaderStageFlag::eFragment );
			m_program->link();
		}
		else
		{
			if ( !wxFileExists( shadersFolder / "shader.vert" )
				|| !wxFileExists( shadersFolder / "shader.frag" ) )
			{
				throw std::runtime_error{ "Shader files are missing" };
			}

			m_program->createModule( common::dumpTextFile( shadersFolder / "shader.vert" )
				, renderer::ShaderStageFlag::eVertex );
			m_program->createModule( common::dumpTextFile( shadersFolder / "shader.frag" )
				, renderer::ShaderStageFlag::eFragment );
			m_program->link();
		}

		m_pipeline = m_device->createPipeline( *m_pipelineLayout
			, *m_program
			, { *m_vertexLayout }
			, *m_renderPass
			, renderer::PrimitiveTopology::eTriangleStrip );
		m_pipeline->multisampleState( renderer::MultisampleState{} );
		m_pipeline->finish();
	}

	void RenderPanel::doPrepareFrames()
	{
		m_commandBuffers = m_swapChain->createCommandBuffers();
		m_frameBuffers = m_swapChain->createFrameBuffers( *m_renderPass );
		auto dimensions = m_swapChain->getDimensions();

		for ( size_t i = 0u; i < m_commandBuffers.size(); ++i )
		{
			auto & frameBuffer = *m_frameBuffers[i];
			auto & commandBuffer = *m_commandBuffers[i];

			if ( commandBuffer.begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
			{
				commandBuffer.beginRenderPass( *m_renderPass
					, frameBuffer
					, { m_swapChain->getClearColour() }
					, renderer::SubpassContents::eInline );
				commandBuffer.bindPipeline( *m_pipeline );
				commandBuffer.setViewport( { uint32_t( dimensions.x )
					, uint32_t( dimensions.y )
					, 0
					, 0 } );
				commandBuffer.setScissor( { 0
					, 0
					, uint32_t( dimensions.x )
					, uint32_t( dimensions.y ) } );
				commandBuffer.bindGeometryBuffers( *m_geometryBuffers );
				commandBuffer.draw( 4u, 1u, 0u, 0u );
				commandBuffer.endRenderPass();

				auto res = commandBuffer.end();

				if ( !res )
				{
					std::stringstream stream;
					stream << "Command buffers recording failed.";
					throw std::runtime_error{ stream.str() };
				}
			}
		}
	}

	void RenderPanel::doDraw()
	{
		auto resources = m_swapChain->getResources();

		if ( resources )
		{
			auto before = std::chrono::high_resolution_clock::now();
			auto & queue = m_device->getGraphicsQueue();
			auto res = queue.submit( *m_commandBuffers[resources->getBackBuffer()]
				, resources->getImageAvailableSemaphore()
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, resources->getRenderingFinishedSemaphore()
				, &resources->getFence() );
			m_swapChain->present( *resources );

			auto after = std::chrono::high_resolution_clock::now();
			wxGetApp().updateFps( std::chrono::duration_cast< std::chrono::microseconds >( after - before ) );
		}
		else
		{
			m_timer->Stop();
		}
	}

	void RenderPanel::doResetSwapChain()
	{
		doPrepareFrames();
	}

	void RenderPanel::onTimer( wxTimerEvent & event )
	{
		if ( event.GetId() == int( Ids::RenderTimer ) )
		{
			doDraw();
		}
	}

	void RenderPanel::onSize( wxSizeEvent & event )
	{
		m_timer->Stop();
		m_device->waitIdle();
		wxSize size{ GetClientSize() };
		m_swapChain->reset( { size.GetWidth(), size.GetHeight() } );
		m_timer->Start( TimerTimeMs );
		event.Skip();
	}
}
