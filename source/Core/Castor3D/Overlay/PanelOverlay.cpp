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

				auto uv = writer.declLocale( "uv"
					, overlayData.uv() );
				auto bounds = writer.declLocale( "bounds"
					, vec4( vec2( 0.0_f )
						, overlayData.absoluteSize() * renderSize ) );
				auto borderExtent = writer.declLocale( "borderExtent"
					, vec4( overlayData.border().xy() * renderSize
						, -overlayData.border().zw() * renderSize ) );

				IF( writer, overlayData.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					bounds += borderExtent;
				}
				ELSEIF( overlayData.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					bounds += borderExtent / 2.0_f;
				}
				FI;

				auto lt = writer.declLocale( "lt", shader::OverlaySurface{ bounds.xy() / renderSize, uv.xw() } );
				auto lb = writer.declLocale( "lb", shader::OverlaySurface{ bounds.xw() / renderSize, uv.xy() } );
				auto rt = writer.declLocale( "rt", shader::OverlaySurface{ bounds.zy() / renderSize, uv.zw() } );
				auto rb = writer.declLocale( "rb", shader::OverlaySurface{ bounds.zw() / renderSize, uv.zy() } );

				uint32_t index = 0;
				c3d_overlaysSurfaces[offset + index] = lt; ++index;
				c3d_overlaysSurfaces[offset + index] = lb; ++index;
				c3d_overlaysSurfaces[offset + index] = rb; ++index;
				c3d_overlaysSurfaces[offset + index] = lt; ++index;
				c3d_overlaysSurfaces[offset + index] = rb; ++index;
				c3d_overlaysSurfaces[offset + index] = rt; ++index;
			} );
		comp.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		return makeShaderState( device, comp );
	}

	void PanelOverlay::doUpdate( OverlayRenderer const & renderer )
	{
	}
}
