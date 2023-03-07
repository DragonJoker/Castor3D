#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslOverlaySurface.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, PanelOverlay )

namespace castor3d
{
	PanelOverlay::PanelOverlay()
		: OverlayCategory( OverlayType::ePanel )
	{
	}

	OverlayCategorySPtr PanelOverlay::create()
	{
		return std::make_shared< PanelOverlay >();
	}

	void PanelOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	uint32_t PanelOverlay::getCount( bool )const
	{
		return 6u;
	}

	ashes::PipelineShaderStageCreateInfo PanelOverlay::createProgram( RenderDevice const & device )
	{
		ShaderModule comp{ VK_SHADER_STAGE_COMPUTE_BIT, "PanelOverlayCompute" };
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
				auto overlayData = writer.declLocale( "overlayData"
					, c3d_overlaysData[in.globalInvocationID.x()] );
				auto offset = writer.declLocale( "offset"
					, overlayData.vertexOffset() );
				auto renderSize = writer.declLocale( "renderSize"
					, vec2( c3d_cameraData.renderSize() ) );

				auto l = writer.declLocale( "l", 0.0_f );
				auto t = writer.declLocale( "t", 0.0_f );
				auto r = writer.declLocale( "r", overlayData.size().x() );
				auto b = writer.declLocale( "b", overlayData.size().y() );

				IF( writer, overlayData.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					l += overlayData.border().x();
					t += overlayData.border().y();
					r -= overlayData.border().z();
					b -= overlayData.border().w();
				}
				ELSEIF( overlayData.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					l += overlayData.border().x() / 2.0_f;
					t += overlayData.border().y() / 2.0_f;
					r -= overlayData.border().z() / 2.0_f;
					b -= overlayData.border().w() / 2.0_f;
				}
				FI;

				auto lt = writer.declLocale( "lt", vec2( l, t ) / renderSize );
				auto lb = writer.declLocale( "lb", vec2( l, b ) / renderSize );
				auto rt = writer.declLocale( "rt", vec2( r, t ) / renderSize );
				auto rb = writer.declLocale( "rb", vec2( r, b ) / renderSize );

				auto ltUV = writer.declLocale( "ltUV", overlayData.uv().xw() );
				auto lbUV = writer.declLocale( "lbUV", overlayData.uv().xy() );
				auto rtUV = writer.declLocale( "rtUV", overlayData.uv().zw() );
				auto rbUV = writer.declLocale( "rbUV", overlayData.uv().zy() );

				uint32_t index = 0;
				c3d_overlaysSurfaces[offset + index].set( lt, ltUV ); ++index;
				c3d_overlaysSurfaces[offset + index].set( lb, lbUV ); ++index;
				c3d_overlaysSurfaces[offset + index].set( rb, rbUV ); ++index;
				c3d_overlaysSurfaces[offset + index].set( lt, ltUV ); ++index;
				c3d_overlaysSurfaces[offset + index].set( rb, rbUV ); ++index;
				c3d_overlaysSurfaces[offset + index].set( rt, rtUV ); ++index;
			} );
		comp.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		return makeShaderState( device, comp );
	}

	void PanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
	}
}
