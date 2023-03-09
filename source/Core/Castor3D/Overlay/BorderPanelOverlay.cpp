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
				auto ssAbsParentSize = writer.declLocale( "ssAbsParentSize"
					, overlay.parentRect().zw() - overlay.parentRect().xy() );
				auto ssAbsSize = writer.declLocale( "ssAbsSize"
					, overlay.relativeSize() * ssAbsParentSize );

				auto ssInner = writer.declLocale( "ssInner"
					, vec4( vec2( 0.0_f ), ssAbsSize ) );
				auto ssOuter = writer.declLocale( "ssOuter"
					, ssInner );

				auto ssBorderExtent = writer.declLocale( "ssBorderExtent"
					, vec4( overlay.border().xy()
						, -overlay.border().zw() ) );

				IF( writer, overlay.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					ssInner += ssBorderExtent;
				}
				ELSEIF( overlay.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					ssInner += ssBorderExtent / 2.0_f;
					ssOuter -= ssBorderExtent / 2.0_f;
				}
				ELSE
				{
					ssOuter -= ssBorderExtent;
				}
				FI;

				auto innerUv = writer.declLocale( "innerUv"
					, overlay.borderInnerUV() );
				auto outerUv = writer.declLocale( "outerUv"
					, overlay.borderOuterUV() );
				auto ssRelPosition = writer.declLocale( "ssRelPosition"
					, overlay.relativePosition() * ssAbsParentSize );

				auto ssInnerX = writer.declLocale( "ssInnerX", ssInner.x() );
				auto ssInnerY = writer.declLocale( "ssInnerY", ssInner.y() );
				auto ssInnerZ = writer.declLocale( "ssInnerZ", ssInner.z() );
				auto ssInnerW = writer.declLocale( "ssInnerW", ssInner.w() );
				auto ssOuterX = writer.declLocale( "ssOuterX", ssOuter.x() );
				auto ssOuterY = writer.declLocale( "ssOuterY", ssOuter.y() );
				auto ssOuterZ = writer.declLocale( "ssOuterZ", ssOuter.z() );
				auto ssOuterW = writer.declLocale( "ssOuterW", ssOuter.w() );

				auto innerUvX = writer.declLocale( "innerUvX", innerUv.x() );
				auto innerUvY = writer.declLocale( "innerUvY", innerUv.y() );
				auto innerUvZ = writer.declLocale( "innerUvZ", innerUv.z() );
				auto innerUvW = writer.declLocale( "innerUvW", innerUv.w() );
				auto outerUvX = writer.declLocale( "outerUvX", outerUv.x() );
				auto outerUvY = writer.declLocale( "outerUvY", outerUv.y() );
				auto outerUvZ = writer.declLocale( "outerUvZ", outerUv.z() );
				auto outerUvW = writer.declLocale( "outerUvW", outerUv.w() );

				overlay.cropMinValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), innerUv.xz(), ssInnerX, innerUvX );
				overlay.cropMinValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), innerUv.yw(), ssInnerY, innerUvY );
				overlay.cropMaxValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), innerUv.xz(), ssInnerZ, innerUvZ );
				overlay.cropMaxValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), innerUv.yw(), ssInnerW, innerUvW );

				overlay.cropMinValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), outerUv.xz(), ssOuterX, outerUvX );
				overlay.cropMinValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), outerUv.yw(), ssOuterY, outerUvY );
				overlay.cropMaxValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), outerUv.xz(), ssOuterZ, outerUvZ );
				overlay.cropMaxValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), outerUv.yw(), ssOuterW, outerUvW );
				
				uint32_t index = 0;

				// Corner Top Left
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssOuterY ), vec2( outerUvX, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerY ), vec2( outerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerY ), vec2( innerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssOuterY ), vec2( outerUvX, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerY ), vec2( innerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterY ), vec2( innerUvX, outerUvY ) ); ++index;

				// Border Top
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterY ), vec2( innerUvX, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerY ), vec2( innerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerY ), vec2( innerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterY ), vec2( innerUvX, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerY ), vec2( innerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterY ), vec2( innerUvZ, outerUvY ) ); ++index;

				// Corner Top Right
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterY ), vec2( innerUvZ, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerY ), vec2( innerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerY ), vec2( outerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterY ), vec2( innerUvZ, outerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerY ), vec2( outerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssOuterY ), vec2( outerUvZ, outerUvY ) ); ++index;

				// Border Left
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerY ), vec2( outerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerW ), vec2( outerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerW ), vec2( innerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerY ), vec2( outerUvX, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerW ), vec2( innerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerY ), vec2( innerUvX, innerUvY ) ); ++index;

				// Border Right
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerY ), vec2( innerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerW ), vec2( innerUvZ, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerW ), vec2( outerUvZ, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerY ), vec2( innerUvZ, innerUvY ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerW ), vec2( outerUvZ, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerY ), vec2( outerUvZ, innerUvY ) ); ++index;

				// Corner Bottom Left
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerW ), vec2( outerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssOuterW ), vec2( outerUvX, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterW ), vec2( innerUvX, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterX, ssInnerW ), vec2( outerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterW ), vec2( innerUvX, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerW ), vec2( innerUvX, innerUvW ) ); ++index;

				// Border Bottom
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerW ), vec2( innerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssOuterW ), vec2( innerUvX, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterW ), vec2( innerUvZ, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerX, ssInnerW ), vec2( innerUvX, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterW ), vec2( innerUvZ, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerW ), vec2( innerUvZ, innerUvW ) ); ++index;

				// Corner Bottom Right
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerW ), vec2( innerUvZ, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssOuterW ), vec2( innerUvZ, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssOuterW ), vec2( outerUvZ, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssInnerZ, ssInnerW ), vec2( innerUvZ, innerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssOuterW ), vec2( outerUvZ, outerUvW ) ); ++index;
				c3d_overlaysSurfaces[offset + index].set( vec2( ssOuterZ, ssInnerW ), vec2( outerUvZ, innerUvW ) ); ++index;
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
