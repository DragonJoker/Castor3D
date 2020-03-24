#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	uint32_t const OverlayUbo::BindingPoint = 3u;
	castor::String const OverlayUbo::BufferOverlayName = cuT( "Overlay" );
	castor::String const OverlayUbo::BufferOverlayInstance = cuT( "overlay" );
	castor::String const OverlayUbo::PositionRatio = cuT( "c3d_positionRatio" );
	castor::String const OverlayUbo::RenderSizeIndex = cuT( "c3d_renderSizeIndex" );
}
