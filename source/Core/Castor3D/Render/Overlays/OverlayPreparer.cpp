/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	namespace ovrlprep
	{
		static castor::Point4f getParentRect( Overlay const & overlay
			, castor::Size const & renderSize )
		{
			if ( !overlay.getParent() )
			{
				return castor::Point4f{ 0.0f, 0.0f, 1.0f, 1.0f };
			}

			auto & parent = *overlay.getParent()->getCategory();
			auto pos = parent.getAbsolutePosition();
			auto dim = parent.getAbsoluteSize();
			auto renderRatio = parent.getRenderRatio( renderSize );
			return castor::Point4f{ renderRatio->x * pos->x
				, renderRatio->y * pos->y
				, renderRatio->x * ( pos->x + dim->x )
				, renderRatio->y * ( pos->y + dim->y ) };
		}

		static castor::Point2d updateUbo( OverlayUboConfiguration & data
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & renderSize
			, uint32_t vertexOffset )
		{
			auto ratio = overlay.getRenderRatio( renderSize );
			data.relativeSize = castor::Point2f{ overlay.getRelativeSize() * ratio };
			data.relativePosition = castor::Point2f{ overlay.getRelativePosition() * ratio };
			data.parentRect = getParentRect( overlay.getOverlay(), renderSize );
			data.renderArea = castor::Point4f{ overlay.computeClientArea()
				* castor::Point4d{ ratio->x, ratio->y, ratio->x, ratio->y } };
			data.uv = castor::Point4f{ overlay.getUV() };
			data.materialId = pass.getId();
			data.vertexOffset = vertexOffset;
			return ratio;
		}

		static size_t makeHash( ashes::Pipeline const * pipeline
			, ashes::DescriptorSetCRefArray const * descriptorSets )
		{
			auto result = std::hash< ashes::Pipeline const * >{}( pipeline );
			result = castor::hashCombinePtr( result, *descriptorSets );
			return result;
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
		m_renderer.doResetCompute();
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
			fillDrawData();
			auto & commandBuffer = m_renderer.doBeginPrepare( m_renderPass, m_framebuffer );

			for ( auto [level, pipelines] : m_levelsOverlays )
			{
				for ( auto [pipelineData, overlayDatas] : pipelines )
				{
					if ( !overlayDatas.empty() )
					{
						auto & data = overlayDatas.front();
						auto & count = m_descriptorsCounts[ovrlprep::makeHash( data.node->pipeline.pipeline.get(), &pipelineData->descriptorSets->all )];
#if !defined( NDEBUG )
						auto commands = castor::makeArrayView( pipelineData->indirectCommands.begin() + count
							, uint32_t( overlayDatas.size() ) );
						for ( auto & command : commands )
						{
							if ( command.vertexCount == 0
								|| command.instanceCount == 0 )
							{
								log::error << "OverlayPreparer: "
									<< "Level " << level
									<< "Unexpected empty draw command" << std::endl;
							}
						}
#endif
						doRegisterDrawCommands( data.node->pipeline
							, pipelineData->descriptorSets->all
							, pipelineData->indirectCommandsBuffer->getBuffer()
							, uint32_t( overlayDatas.size() )
							, count
							, commandBuffer );;
					}
				}
			}

			m_renderer.doEndPrepare();
		}
	}

	OverlaysCounts OverlayPreparer::registerOverlay( Overlay const & overlay )
	{
		OverlaysCounts counts{};

		if ( auto material = overlay.getMaterial() )
		{
			auto size = overlay.getAbsoluteSize( m_renderer.getSize() );

			if ( size->x > 0 && size->y > 0 )
			{
				auto & pipelines = m_levelsOverlays.emplace( overlay.getLevel(), OverlayDatasMap{} ).first->second;

				for ( auto & pass : *material )
				{
					if ( !pass->isImplicit() )
					{
						counts.quads += overlay.getType() == OverlayType::eText
							? overlay.getTextOverlay()->getCharCount()
							: 1u;
						counts.overlays++;
						auto [node, pipelineData] = m_renderer.doGetDrawNodeData( m_device
							, m_renderPass
							, overlay
							, *pass
							, false );
						auto & overlays = pipelines.emplace( pipelineData, OverlayDataArray{} ).first->second;
						overlays.push_back( { &overlay
							, node
							, pipelineData
							, nullptr
							, uint32_t{}
							, uint32_t{}
							, OverlayTextBufferIndex{}
						, false } );
					}
				}
			}
		}

		if ( overlay.getType() == OverlayType::eBorderPanel )
		{
			if ( auto borderMaterial = overlay.getBorderPanelOverlay()->getBorderMaterial() )
			{
				auto borderSize = overlay.getBorderPanelOverlay()->getAbsoluteBorderSize( m_renderer.getSize() );

				if ( borderSize->x != 0 || borderSize->y != 0 || borderSize->z != 0 || borderSize->w != 0 )
				{
					auto & pipelines = m_levelsOverlays.emplace( overlay.getLevel(), OverlayDatasMap{} ).first->second;

					for ( auto & pass : *borderMaterial )
					{
						if ( !pass->isImplicit() )
						{
							counts.quads += 8u;
							counts.overlays++;
							auto [node, pipelineData] = m_renderer.doGetDrawNodeData( m_device
								, m_renderPass
								, overlay
								, *pass
								, true );
							auto & overlays = pipelines.emplace( pipelineData, OverlayDataArray{} ).first->second;
							overlays.push_back( { &overlay
								, node
								, pipelineData
								, nullptr
								, uint32_t{}
								, uint32_t{}
								, OverlayTextBufferIndex{}
							, true } );
						}
					}
				}
			}
		}

		return counts;
	}

	void OverlayPreparer::fillDrawData()
	{
		for ( auto [level, pipelines] : m_levelsOverlays )
		{
			for ( auto [pipelineData, overlayDatas] : pipelines )
			{
				for ( auto data : overlayDatas )
				{
					auto overlay = data.overlay;

					switch ( overlay->getType() )
					{
					case OverlayType::ePanel:
						if ( auto panel = overlay->getPanelOverlay() )
						{
							if ( m_renderer.m_common.panelVertexBuffer->fill( m_renderer.getSize()
								, *panel
								, data
								, false
								, nullptr ) )
							{
								doUpdateUbo( m_renderer.m_common.panelVertexBuffer->overlaysBuffer[data.overlayIndex]
									, *panel
									, data.node->pass
									, m_renderer.getSize()
									, data.indirectData->firstVertex
									, data.textBuffer );
							}
						}
						break;
					case OverlayType::eBorderPanel:
						if ( auto border = overlay->getBorderPanelOverlay() )
						{
							if ( data.secondary )
							{
								if ( m_renderer.m_common.borderVertexBuffer->fill( m_renderer.getSize()
									, *border
									, data
									, true
									, nullptr ) )
								{
									doUpdateUbo( m_renderer.m_common.borderVertexBuffer->overlaysBuffer[data.overlayIndex]
										, *border
										, data.node->pass
										, m_renderer.getSize()
										, data.indirectData->firstVertex
										, data.textBuffer );
								}
							}
							else if ( m_renderer.m_common.panelVertexBuffer->fill( m_renderer.getSize()
								, *border
								, data
								, false
								, nullptr ) )
							{
								doUpdateUbo( m_renderer.m_common.panelVertexBuffer->overlaysBuffer[data.overlayIndex]
									, *border
									, data.node->pass
									, m_renderer.getSize()
									, data.indirectData->firstVertex
									, data.textBuffer );
							}
						}
						break;
					case OverlayType::eText:
						if ( auto text = overlay->getTextOverlay() )
						{
							if ( m_renderer.m_common.textVertexBuffer->fill( m_renderer.getSize()
								, *text
								, data
								, false
								, text->getFontTexture().get() ) )
							{
								doUpdateUbo( m_renderer.m_common.textVertexBuffer->overlaysBuffer[data.overlayIndex]
									, *text
									, data.node->pass
									, m_renderer.getSize()
									, data.indirectData->firstVertex
									, data.textBuffer );
							}
						}
						break;
					default:
						break;
					}

					m_overlays.push_back( std::move( data ) );
				}
			}
		}
	}

	void OverlayPreparer::doRegisterDrawCommands( OverlayDrawPipeline const & pipeline
		, ashes::DescriptorSetCRefArray const & descriptorSets
		, ashes::BufferBase const & indirectCommands
		, uint32_t drawCount
		, uint32_t & offset
		, ashes::CommandBuffer & commandBuffer )
	{
		commandBuffer.bindPipeline( *pipeline.pipeline );
		commandBuffer.bindDescriptorSets( descriptorSets, *pipeline.pipelineLayout );
		DrawConstants constants{ offset };
		commandBuffer.pushConstants( *pipeline.pipelineLayout
			, VK_SHADER_STAGE_VERTEX_BIT
			, 0u
			, sizeof( constants )
			, &constants );
		commandBuffer.drawIndirect( indirectCommands
			, uint32_t( offset * sizeof( VkDrawIndirectCommand ) )
			, drawCount
			, sizeof( VkDrawIndirectCommand ) );
		offset += drawCount;

		if ( m_drawCounts )
		{
			++( *m_drawCounts );
		}
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
			, vertexOffset );
		data.border = {};
		data.borderInnerUV = {};
		data.borderOuterUV = {};
		data.borderPosition = {};
		data.textTexturingMode = {};
		data.textWordOffset = {};
		data.textLineOffset = {};
		data.textTopOffset = {};
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
			, vertexOffset );
		data.border = castor::Point4f{ overlay.getAbsoluteBorderSize()
			* castor::Point4d{ ratio->x, ratio->y, ratio->x, ratio->y } };
		data.borderInnerUV = castor::Point4f{ overlay.getBorderInnerUV() };
		data.borderOuterUV = castor::Point4f{ overlay.getBorderOuterUV() };
		data.borderPosition = uint32_t( overlay.getBorderPosition() );
		data.textTexturingMode = {};
		data.textWordOffset = {};
		data.textLineOffset = {};
		data.textTopOffset = {};
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
			, vertexOffset );
		data.border = {};
		data.borderInnerUV = {};
		data.borderOuterUV = {};
		data.borderPosition = {};
		data.textTexturingMode = uint32_t( overlay.getTexturingMode() );
		data.textWordOffset = textBuffer.word;
		data.textLineOffset = textBuffer.line;
		data.textTopOffset = textBuffer.top;
	}
}
