#include "Castor3D/Overlay/PanelOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslOverlaySurface.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, PanelOverlay )

namespace c3d
{
	PanelOverlay::PanelOverlay()
		: OverlayCategory( OverlayType::ePanel )
	{
	}

	OverlayCategoryUPtr PanelOverlay::create()
	{
		return makeUniqueDerived< OverlayCategory, PanelOverlay >();
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
		auto & engine = c3d::getEngine( device );
		ShaderModule comp{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "PanelOverlayCompute" ) };
		sdw::ComputeWriter writer{ &engine.getShaderAllocator() };
		{
			C3D_Camera( writer
				, uint32_t( ComputeBindingIdx::eCamera )
				, 0u );
			C3D_Overlays( writer
				, uint32_t( ComputeBindingIdx::eOverlays )
				, 0u );
			C3D_OverlaysSurfaces( writer
				, uint32_t( ComputeBindingIdx::eVertex )
				, 0u
				, false
				, true );

			auto pcb = writer.declPushConstantsBuffer( "c3d_overlaysConstants" );
			auto c3d_overlaysCount = pcb.declMember< sdw::UInt >( "c3d_overlaysCount" );
			pcb.end();

			auto threadCount = engine.isRenderDocSupportEnabled() ? 1U : 256U;
			writer.implementMain( threadCount
				, [&writer, &c3d_overlaysCount, &c3d_overlaysData, &c3d_overlaysSurfaces]( sdw::ComputeIn const & in )
				{
					sdwIF( writer, in.globalInvocationID.x() < c3d_overlaysCount )
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
						auto ssBorderExtent = writer.declLocale( "ssBorderExtent"
							, vec4( overlay.border().xy()
								, -overlay.border().zw() ) );

						sdwIF( writer, overlay.borderPosition() == uint32_t( BorderPosition::eInternal ) )
						{
							ssRelBounds += ssBorderExtent;
						}
						sdwELSEIF( overlay.borderPosition() == uint32_t( BorderPosition::eMiddle ) )
						{
							ssRelBounds += ssBorderExtent / 2.0_f;
						}
						sdwFI

						auto ssRelOvPosition = writer.declLocale( "ssRelOvPosition"
							, overlay.relativePosition() * ssAbsParentSize );
						auto ssAbsOvPosition = writer.declLocale( "ssAbsOvPosition"
							, ssRelOvPosition + overlay.parentRect().xy() );
						auto srcUv = writer.declLocale( "srcUv"
							, uv );

						overlay.cropMinValue( ssAbsOvPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), overlay.scissorRect().xz(), srcUv.xz(), ssRelBounds.x(), uv.x() );
						overlay.cropMaxValue( ssAbsOvPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), overlay.scissorRect().yw(), srcUv.yw(), ssRelBounds.y(), uv.w() );
						overlay.cropMaxValue( ssAbsOvPosition.x(), ssAbsParentSize.x(), ssAbsSize.x(), overlay.scissorRect().xz(), srcUv.xz(), ssRelBounds.z(), uv.z() );
						overlay.cropMinValue( ssAbsOvPosition.y(), ssAbsParentSize.y(), ssAbsSize.y(), overlay.scissorRect().yw(), srcUv.yw(), ssRelBounds.w(), uv.y() );

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
					}
					sdwFI
				} );
			comp.shader = writer.getBuilder().releaseShader();
		}
		return makeShaderState( device, comp );
	}
}
