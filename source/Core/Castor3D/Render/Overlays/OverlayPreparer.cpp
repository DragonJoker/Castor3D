/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"

#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"

namespace castor3d
{
	namespace ovrlprep
	{
		static castor::Point2d updateUbo( OverlayUboConfiguration & data
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & renderSize
			, uint32_t vertexOffset )
		{
			auto size = overlay.getAbsoluteSize( renderSize );
			auto ratio = overlay.getRenderRatio( renderSize );
			data.size = castor::Point2f{ size.getWidth(), size.getHeight() };
			data.position = castor::Point2f{ overlay.getAbsolutePosition() };
			data.size = castor::Point2f{ castor::Point2d{ data.size } *ratio };
			data.uv = castor::Point4f{ overlay.getUV() };
			data.materialId = pass.getId();
			data.vertexOffset = vertexOffset;
			return ratio;
		}
	}

	OverlayPreparer::OverlayPreparer( OverlayRenderer & renderer
		, RenderDevice const & device
		, VkRenderPass renderPass
		, VkFramebuffer framebuffer )
		: m_renderer{ renderer }
		, m_device{ device }
		, m_renderPass{ renderPass }
		, m_framebuffer{ framebuffer }
	{
		m_renderer.m_computePanelPipeline.count = 0u;
		m_renderer.m_computeBorderPipeline.count = 0u;

		for ( auto & it : m_renderer.m_computeTextPipeline.sets )
		{
			it.second.count = 0u;
		}
	}

	OverlayPreparer::OverlayPreparer( OverlayPreparer && rhs )noexcept
		: m_renderer{ rhs.m_renderer }
		, m_device{ rhs.m_device }
		, m_renderPass{ rhs.m_renderPass }
		, m_framebuffer{ rhs.m_framebuffer }
	{
		rhs.m_renderPass = VkRenderPass{};
		rhs.m_framebuffer = VkFramebuffer{};
	}

	OverlayPreparer & OverlayPreparer::operator=( OverlayPreparer && rhs )noexcept
	{
		m_renderPass = rhs.m_renderPass;

		rhs.m_renderPass = VkRenderPass{};
		rhs.m_framebuffer = VkFramebuffer{};

		return *this;
	}

	OverlayPreparer::~OverlayPreparer()noexcept
	{
		if ( m_renderPass )
		{
			prepare();
			m_renderer.doBeginPrepare( m_renderPass, m_framebuffer );
			ashes::CommandBuffer & commandBuffer = *m_renderer.m_commands.commandBuffer;

			for ( auto overlayIt : m_overlays )
			{
				doPrepareOverlayCommands( overlayIt, commandBuffer );
			}

			m_renderer.doEndPrepare();
		}
	}

	void OverlayPreparer::fill( Overlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			auto & pipelines = m_levelsOverlays.emplace( overlay.getLevel(), OverlayDatasMap{} ).first->second;

			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
				{
					OverlayRenderNode * node{};
					ashes::DescriptorSetCRefArray const * descriptorSets{};

					switch ( overlay.getType() )
					{
					case OverlayType::ePanel:
						node = &m_renderer.doGetPanelNode( m_device, m_renderPass, *pass );
						descriptorSets = &m_renderer.m_panelVertexBuffer->getDrawDescriptorSets( *node, nullptr, nullptr );
						break;
					case OverlayType::eBorderPanel:
						node = &m_renderer.doGetPanelNode( m_device, m_renderPass, *pass );
						descriptorSets = &m_renderer.m_panelVertexBuffer->getDrawDescriptorSets( *node, nullptr, nullptr );
						break;
					case OverlayType::eText:
						{
							auto texture = overlay.getTextOverlay()->getFontTexture();
							node = &m_renderer.doGetTextNode( m_device, m_renderPass, *pass, *texture->getTexture(), *texture->getSampler().lock() );
							descriptorSets = &m_renderer.m_textVertexBuffer->getDrawDescriptorSets( *node
								, texture.get()
								, &m_renderer.doCreateTextDescriptorSet( *texture ) );
						}
						break;
					default:
						break;
					}

					if ( node )
					{
						auto & overlays = pipelines.emplace( &node->pipeline, OverlayDataArray{} ).first->second;
						overlays.emplace_back( &overlay
							, node
							, descriptorSets
							, OverlayGeometryBuffers{}
							, uint32_t{}
							, false );
					}
				}
			}
		}

		if ( overlay.getType() == OverlayType::eBorderPanel )
		{
			if ( auto borderMaterial = overlay.getBorderPanelOverlay()->getBorderMaterial() )
			{
				auto & pipelines = m_levelsOverlays.emplace( overlay.getLevel(), OverlayDatasMap{} ).first->second;

				for ( auto & pass : *borderMaterial )
				{
					if ( !pass->isImplicit() )
					{
						auto node = &m_renderer.doGetPanelNode( m_device, m_renderPass, *pass );
						auto descriptorSets = &m_renderer.m_borderVertexBuffer->getDrawDescriptorSets( *node, nullptr, nullptr );
						auto & overlays = pipelines.emplace( &node->pipeline, OverlayDataArray{} ).first->second;
						overlays.emplace_back( &overlay
							, node
							, descriptorSets
							, OverlayGeometryBuffers{}
							, uint32_t{}
							, true );
					}
				}
			}
		}
	}

	void OverlayPreparer::prepare()
	{
		for ( auto [level, pipelines] : m_levelsOverlays )
		{
			for ( auto [pipeline, overlayDatas] : pipelines )
			{
				for ( auto data : overlayDatas )
				{
					auto overlay = data.overlay;

					switch ( overlay->getType() )
					{
					case OverlayType::ePanel:
						if ( auto cat = overlay->getPanelOverlay() )
						{
							doPrepareOverlayDescriptors< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
								, *cat
								, std::move( data )
								, *m_renderer.m_panelVertexBuffer
								, nullptr
								, false );
							++m_renderer.m_computePanelPipeline.count;
						}
						break;
					case OverlayType::eBorderPanel:
						if ( auto cat = overlay->getBorderPanelOverlay() )
						{
							if ( data.secondary )
							{
								doPrepareOverlayDescriptors< OverlayRenderer::BorderPanelVertexBufferPool::Quad >( m_device
									, *cat
									, std::move( data )
									, *m_renderer.m_borderVertexBuffer
									, nullptr
									, true );
								++m_renderer.m_computeBorderPipeline.count;
							}
							else
							{
								doPrepareOverlayDescriptors< OverlayRenderer::PanelVertexBufferPool::Quad >( m_device
									, *cat
									, std::move( data )
									, *m_renderer.m_panelVertexBuffer
									, nullptr
									, false );
								++m_renderer.m_computePanelPipeline.count;
							}
						}
						break;
					case OverlayType::eText:
						if ( auto cat = overlay->getTextOverlay() )
						{
							doPrepareOverlayDescriptors< OverlayRenderer::TextVertexBufferPool::Quad >( m_device
								, *cat
								, std::move( data )
								, *m_renderer.m_textVertexBuffer
								, cat->getFontTexture().get()
								, false );
							m_renderer.doGetComputeTextPipeline( *cat->getFontTexture() ).count += cat->getCharCount();
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}

	void OverlayPreparer::doPrepareOverlayCommands( OverlayData const & data
		, ashes::CommandBuffer & commandBuffer )
	{
		if ( m_previousPipeline != data.node->pipeline.pipeline.get() )
		{
			commandBuffer.bindPipeline( *data.node->pipeline.pipeline );
			m_previousPipeline = data.node->pipeline.pipeline.get();
			m_previousDescriptorSets = nullptr;
		}

		if ( m_previousDescriptorSets != data.descriptorSets )
		{
			commandBuffer.bindDescriptorSets( *data.descriptorSets, *data.node->pipeline.pipelineLayout );
			m_previousDescriptorSets = data.descriptorSets;
		}

		DrawConstants constants{ data.index };
		commandBuffer.pushConstants( *data.node->pipeline.pipelineLayout
			, VK_SHADER_STAGE_VERTEX_BIT
			, 0u
			, sizeof( constants )
			, &constants );
		commandBuffer.bindVertexBuffer( 0u
			, data.geometryBuffers.buffer->getBuffer().getBuffer()
			, data.geometryBuffers.offset );
		commandBuffer.setScissor( data.overlay->computeScissor( m_renderer.m_size ) );
		commandBuffer.draw( data.geometryBuffers.count
			, 1u
			, 0u
			, 0u );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, PanelOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize
		, uint32_t vertexOffset
		, OverlayTextBufferIndex const & textBuffer )const
	{
		ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize
			, vertexOffset / sizeof( PanelOverlay::Vertex ) );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, BorderPanelOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize
		, uint32_t vertexOffset
		, OverlayTextBufferIndex const & textBuffer )const
	{
		auto ratio = ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize
			, vertexOffset / sizeof( BorderPanelOverlay::Vertex ) );
		auto border = overlay.getAbsoluteBorderSize( renderSize );
		data.border = castor::Point4f{ border.left(), border.top(), border.right(), border.bottom() };
		data.border = castor::Point4f{ castor::Point4d{ data.border } *castor::Point4d{ ratio->x, ratio->y, ratio->x, ratio->y } };
		data.borderInnerUV = castor::Point4f{ overlay.getBorderInnerUV() };
		data.borderOuterUV = castor::Point4f{ overlay.getBorderOuterUV() };
		data.borderPosition = uint32_t( overlay.getBorderPosition() );
	}

	void OverlayPreparer::doUpdateUbo( OverlayUboConfiguration & data
		, TextOverlay const & overlay
		, Pass const & pass
		, castor::Size const & renderSize
		, uint32_t vertexOffset
		, OverlayTextBufferIndex const & textBuffer )const
	{
		ovrlprep::updateUbo( data
			, static_cast< OverlayCategory const & >( overlay )
			, pass
			, renderSize
			, vertexOffset / sizeof( TextOverlay::Vertex ) );
		data.textTexturingMode = uint32_t( overlay.getTexturingMode() );
		data.textWordOffset = textBuffer.word;
		data.textLineOffset = textBuffer.line;
		data.textTopOffset = textBuffer.top;
	}
}
