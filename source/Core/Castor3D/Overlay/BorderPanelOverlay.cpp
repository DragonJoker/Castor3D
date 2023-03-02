#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Shader/Program.hpp"
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

	void BorderPanelOverlay::doUpdateSize( OverlayRenderer const & renderer )
	{
		OverlayCategory::doUpdateSize( renderer );

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
				changed = changed || ( ptSizes[1] != double(  sizes.top() ) / totalSize[1] );
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

		class PanelVertex
			: public sdw::StructInstanceHelperT< "C3D_PanelVertex"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec2Field< "position" >
			, sdw::Vec2Field< "uv" > >
		{
		public:
			PanelVertex( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto position()const {
				return getMember< "position" >();
			}
			auto uv()const {
				return getMember< "uv" >();
			}
		};

		C3D_Camera( writer
			, 0u
			, 0u );
		C3D_Overlays( writer
			, 1u
			, 0u );
		auto c3d_vertexData = writer.declArrayStorageBuffer< PanelVertex >( "c3d_vertexData"
			, 2u
			, 0u );

		writer.implementMain( 1u
			, [&]( sdw::ComputeIn in )
			{
				auto overlayData = writer.declLocale( "overlayData"
				, c3d_overlaysData[in.globalInvocationID.x()] );
				auto offset = writer.declLocale( "offset"
					, in.globalInvocationID.x() * 48u );
				auto renderSize = writer.declLocale( "renderSize"
					, vec2( c3d_cameraData.renderSize() ) );
				auto ww = writer.declLocale( "w"
					, renderSize.x() );
				auto hh = writer.declLocale( "h"
					, renderSize.y() );

				auto centerL = writer.declLocale( "centerL", 0.0_f );
				auto centerT = writer.declLocale( "centerT", 0.0_f );
				auto centerR = writer.declLocale( "centerR", overlayData.size().x() );
				auto centerB = writer.declLocale( "centerB", overlayData.size().y() );

				IF( writer, overlayData.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					centerL += overlayData.border().x();
					centerT += overlayData.border().y();
					centerR -= overlayData.border().z();
					centerB -= overlayData.border().w();
				}
				ELSEIF( overlayData.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					centerL += overlayData.border().x() / 2.0_f;
					centerT += overlayData.border().y() / 2.0_f;
					centerR -= overlayData.border().z() / 2.0_f;
					centerB -= overlayData.border().w() / 2.0_f;
				}
				FI;

				uint32_t index = 0;
				auto borderL = writer.declLocale( "borderL", ( centerL - overlayData.border().x() ) / ww );
				auto borderT = writer.declLocale( "borderT", ( centerT - overlayData.border().y() ) / hh );
				auto borderR = writer.declLocale( "borderR", ( centerR + overlayData.border().z() ) / ww );
				auto borderB = writer.declLocale( "borderB", ( centerB + overlayData.border().w() ) / hh );
				centerL /= ww;
				centerT /= hh;
				centerR /= ww;
				centerB /= hh;

				auto borderUvLL = writer.declLocale( "borderUvLL", overlayData.borderOuterUV().x() );
				auto borderUvTT = writer.declLocale( "borderUvTT", overlayData.borderOuterUV().y() );
				auto borderUvML = writer.declLocale( "borderUvML", overlayData.borderInnerUV().x() );
				auto borderUvMT = writer.declLocale( "borderUvMT", overlayData.borderInnerUV().y() );
				auto borderUvMR = writer.declLocale( "borderUvMR", overlayData.borderInnerUV().z() );
				auto borderUvMB = writer.declLocale( "borderUvMB", overlayData.borderInnerUV().w() );
				auto borderUvRR = writer.declLocale( "borderUvRR", overlayData.borderOuterUV().z() );
				auto borderUvBB = writer.declLocale( "borderUvBB", overlayData.borderOuterUV().w() );

				// Corner Top Left
				c3d_vertexData[offset + index].position() = vec2( borderL, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvTT ); ++index;

				// Border Top
				c3d_vertexData[offset + index].position() = vec2( centerL, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvTT ); ++index;

				// Corner Top Right
				c3d_vertexData[offset + index].position() = vec2( centerR, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvTT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, borderT ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvTT ); ++index;

				// Border Left
				c3d_vertexData[offset + index].position() = vec2( borderL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMT ); ++index;

				// Border Right
				c3d_vertexData[offset + index].position() = vec2( centerR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMT ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerT ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMT ); ++index;

				// Corner Bottom Left
				c3d_vertexData[offset + index].position() = vec2( borderL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvLL, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMB ); ++index;

				// Border Bottom
				c3d_vertexData[offset + index].position() = vec2( centerL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerL, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvML, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMB ); ++index;

				// Corner Bottom Right
				c3d_vertexData[offset + index].position() = vec2( centerR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( centerR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvMR, borderUvMB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, borderB ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvBB ); ++index;
				c3d_vertexData[offset + index].position() = vec2( borderR, centerB ); c3d_vertexData[offset + index].uv() = vec2( borderUvRR, borderUvMB ); ++index;
			} );
		comp.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		return makeShaderState( device, comp );
	}

	void BorderPanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		m_borderChanged = false;
	}
}
