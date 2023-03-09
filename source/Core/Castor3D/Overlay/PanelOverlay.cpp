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
				auto overlay = writer.declLocale( "overlay"
					, c3d_overlaysData[in.globalInvocationID.x()] );
				auto offset = writer.declLocale( "offset"
					, overlay.vertexOffset() );

				auto uv = writer.declLocale( "uv"
					, overlay.uv() );
				auto ssAbsParentSize = writer.declLocale( "ssAbsParentSize"
					, overlay.parentRect().zw() - overlay.parentRect().xy() );
				auto ssAbsSize = writer.declLocale( "ssAbsSize"
					, overlay.relativeSize() * ssAbsParentSize );
				auto ssRelBounds = writer.declLocale( "ssRelBounds"
					, vec4( vec2( 0.0_f ), ssAbsSize ) );
				auto borderExtent = writer.declLocale( "borderExtent"
					, vec4( overlay.border().xy()
						, -overlay.border().zw() ) );

				IF( writer, overlay.borderPosition() == uint32_t( BorderPosition::eInternal ) )
				{
					ssRelBounds += borderExtent;
				}
				ELSEIF( overlay.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
				{
					ssRelBounds += borderExtent / 2.0_f;
				}
				FI;

				auto ssRelPosition = writer.declLocale( "ssRelPosition"
					, overlay.relativePosition() * ssAbsParentSize );
				auto srcUv = writer.declLocale( "srcUv"
					, uv );

				overlay.cropMinValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), srcUv.xz(), ssRelBounds.x(), uv.x() );
				overlay.cropMaxValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), srcUv.yw(), ssRelBounds.y(), uv.w() );
				overlay.cropMaxValue( ssRelPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), srcUv.xz(), ssRelBounds.z(), uv.z() );
				overlay.cropMinValue( ssRelPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), srcUv.yw(), ssRelBounds.w(), uv.y() );

				auto lt = writer.declLocale( "lt", shader::OverlaySurface{ ssRelBounds.xy(), uv.xw() } );
				auto lb = writer.declLocale( "lb", shader::OverlaySurface{ ssRelBounds.xw(), uv.xy() } );
				auto rt = writer.declLocale( "rt", shader::OverlaySurface{ ssRelBounds.zy(), uv.zw() } );
				auto rb = writer.declLocale( "rb", shader::OverlaySurface{ ssRelBounds.zw(), uv.zy() } );

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
