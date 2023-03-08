#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslOverlaySurface.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, BorderPanelOverlay )

namespace castor3d
{
	BorderPanelOverlay::BorderPanelOverlay()
		: OverlayCategory( OverlayType::eBorderPanel )
		, m_borderOuterUv( 0, 0, 1, 1 )
		, m_borderInnerUv( 0.33, 0.33, 0.66, 0.66 )
		, m_borderChanged( true )
	{
	}

	OverlayCategorySPtr BorderPanelOverlay::create()
	{
		return std::make_shared< BorderPanelOverlay >();
	}

	void BorderPanelOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	uint32_t BorderPanelOverlay::getCount( bool borders )const
	{
		if ( borders )
		{
			return 48u;
		}

		return 6u;
	}

	void BorderPanelOverlay::setBorderMaterial( MaterialRPtr material )
	{
		m_pBorderMaterial = material;
	}

	castor::Rectangle BorderPanelOverlay::getAbsoluteBorderSize( castor::Size const & size )const
	{
		auto absoluteSize = getAbsoluteBorderSize();

		return castor::Rectangle(
				   int32_t( absoluteSize[0] * size.getWidth() ),
				   int32_t( absoluteSize[1] * size.getHeight() ),
				   int32_t( absoluteSize[2] * size.getWidth() ),
				   int32_t( absoluteSize[3] * size.getHeight() )
			   );
	}

	castor::Point4d BorderPanelOverlay::getAbsoluteBorderSize()const
	{
		castor::Point4d absoluteSize = getBorderSize();
		auto parent = getOverlay().getParent();

		if ( parent )
		{
			castor::Point2d parentSize = parent->getAbsoluteSize();
			absoluteSize[0] *= parentSize[0];
			absoluteSize[1] *= parentSize[1];
			absoluteSize[2] *= parentSize[0];
			absoluteSize[3] *= parentSize[1];
		}

		return absoluteSize;
	}

	ashes::PipelineShaderStageCreateInfo BorderPanelOverlay::createProgram( RenderDevice const & device )
	{
		ShaderModule comp{ VK_SHADER_STAGE_COMPUTE_BIT, "BorderOverlayCompute" };
		sdw::ComputeWriter writer;

		C3D_Camera( writer
			, 0u
			, 0u );
		C3D_Overlays( writer
			, 1u
			, 0u );
		C3D_OverlaysSurfaces( writer
			, 2u
			, 0u
			, false
			, true );

		writer.implementMain( 1u
			, [&]( sdw::ComputeIn in )
			{
				auto overlay = writer.declLocale( "overlay"
				, c3d_overlaysData[in.globalInvocationID.x()] );
				auto offset = writer.declLocale( "offset"
					, overlay.vertexOffset() );
				auto renderSize = writer.declLocale( "renderSize"
					, vec2( c3d_cameraData.renderSize() ) );
				auto parentSize = writer.declLocale( "parentSize"
					, overlay.parentRect().zw() - overlay.parentRect().xy() );

				auto center = writer.declLocale( "center"
					, vec4( vec2( 0.0_f )
						, overlay.relativeSize() * parentSize ) );
				auto border = writer.declLocale( "border"
					, center );
				auto borderExtent = writer.declLocale( "borderExtent"
					, vec4( overlay.border().xy()
						, -overlay.border().zw() ) );

				IF( writer, overlay.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					center += borderExtent;
				}
				ELSEIF( overlay.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					center += borderExtent / 2.0_f;
					border -= borderExtent / 2.0_f;
				}
				ELSE
				{
					border -= borderExtent;
				}
				FI;

				uint32_t index = 0;

				auto borderUvLL = writer.declLocale( "borderUvLL", overlay.borderOuterUV().x() );
				auto borderUvTT = writer.declLocale( "borderUvTT", overlay.borderOuterUV().y() );
				auto borderUvML = writer.declLocale( "borderUvML", overlay.borderInnerUV().x() );
				auto borderUvMT = writer.declLocale( "borderUvMT", overlay.borderInnerUV().y() );
				auto borderUvMR = writer.declLocale( "borderUvMR", overlay.borderInnerUV().z() );
				auto borderUvMB = writer.declLocale( "borderUvMB", overlay.borderInnerUV().w() );
				auto borderUvRR = writer.declLocale( "borderUvRR", overlay.borderOuterUV().z() );
				auto borderUvBB = writer.declLocale( "borderUvBB", overlay.borderOuterUV().w() );

				// Corner Top Left
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), border.y() ), vec2( borderUvLL, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.y() ), vec2( borderUvLL, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.y() ), vec2( borderUvML, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), border.y() ), vec2( borderUvLL, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.y() ), vec2( borderUvML, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.y() ), vec2( borderUvML, borderUvTT ) ); ++index;

				// Border Top
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.y() ), vec2( borderUvML, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.y() ), vec2( borderUvML, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.y() ), vec2( borderUvMR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.y() ), vec2( borderUvML, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.y() ), vec2( borderUvMR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.y() ), vec2( borderUvMR, borderUvTT ) ); ++index;

				// Corner Top Right
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.y() ), vec2( borderUvMR, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.y() ), vec2( borderUvMR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.y() ), vec2( borderUvRR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.y() ), vec2( borderUvMR, borderUvTT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.y() ), vec2( borderUvRR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), border.y() ), vec2( borderUvRR, borderUvTT ) ); ++index;

				// Border Left
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.y() ), vec2( borderUvLL, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.w() ), vec2( borderUvLL, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.w() ), vec2( borderUvML, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.y() ), vec2( borderUvLL, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.w() ), vec2( borderUvML, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.y() ), vec2( borderUvML, borderUvMT ) ); ++index;

				// Border Right
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.y() ), vec2( borderUvMR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.w() ), vec2( borderUvMR, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.w() ), vec2( borderUvRR, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.y() ), vec2( borderUvMR, borderUvMT ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.w() ), vec2( borderUvRR, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.y() ), vec2( borderUvRR, borderUvMT ) ); ++index;

				// Corner Bottom Left
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.w() ), vec2( borderUvLL, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), border.w() ), vec2( borderUvLL, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.w() ), vec2( borderUvML, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.x(), center.w() ), vec2( borderUvLL, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.w() ), vec2( borderUvML, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.w() ), vec2( borderUvML, borderUvMB ) ); ++index;

				// Border Bottom
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.w() ), vec2( borderUvML, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), border.w() ), vec2( borderUvML, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.w() ), vec2( borderUvMR, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.x(), center.w() ), vec2( borderUvML, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.w() ), vec2( borderUvMR, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.w() ), vec2( borderUvMR, borderUvMB ) ); ++index;

				// Corner Bottom Right
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.w() ), vec2( borderUvMR, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), border.w() ), vec2( borderUvMR, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), border.w() ), vec2( borderUvRR, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( center.z(), center.w() ), vec2( borderUvMR, borderUvMB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), border.w() ), vec2( borderUvRR, borderUvBB ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( border.z(), center.w() ), vec2( borderUvRR, borderUvMB ) ); ++index;
			} );
		comp.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		return makeShaderState( device, comp );
	}

	void BorderPanelOverlay::doUpdateSize( OverlayRenderer const & renderer )
	{
		if ( isSizeChanged() || isChanged() || renderer.isSizeChanged() )
		{
			auto parent = getOverlay().getParent();
			auto sz = renderer.getSize();
			castor::Point2d totalSize( sz.getWidth(), sz.getHeight() );

			if ( parent )
			{
				auto parentSize = parent->getAbsoluteSize();
				totalSize[0] = parentSize[0] * totalSize[0];
				totalSize[1] = parentSize[1] * totalSize[1];
			}

			auto sizes = getBorderPixelSize();
			auto ptSizes = getBorderSize();
			bool changed = m_borderChanged;

			if ( sizes.left() )
			{
				changed = changed || ( ptSizes[0] != double( sizes.left() ) / totalSize[0] );
				ptSizes[0] = sizes.left() / totalSize[0];
			}

			if ( sizes.top() )
			{
				changed = changed || ( ptSizes[1] != double( sizes.top() ) / totalSize[1] );
				ptSizes[1] = sizes.top() / totalSize[1];
			}

			if ( sizes.right() )
			{
				changed = changed || ( ptSizes[2] != double( sizes.right() ) / totalSize[0] );
				ptSizes[2] = sizes.right() / totalSize[0];
			}

			if ( sizes.bottom() )
			{
				changed = changed || ( ptSizes[3] != double( sizes.bottom() ) / totalSize[1] );
				ptSizes[3] = sizes.bottom() / totalSize[1];
			}

			if ( changed )
			{
				setBorderSize( ptSizes );
			}
		}
	}

	void BorderPanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		m_borderChanged = false;
	}
}
