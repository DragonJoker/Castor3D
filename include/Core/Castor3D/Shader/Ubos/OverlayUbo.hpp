/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

namespace castor3d
{
	class OverlayUbo
	{
	private:
		using Configuration = OverlayUboConfiguration;

	public:
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayName;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlayInstance;
		//!\~english	Name of the overlay position (xy) and render ratio (zw) frame variable.
		//!\~french		Nom de la frame variable contenant la position (xy) et le ratio de rendu (zw) de l'incrustation.
		C3D_API static castor::String const PositionRatio;
		//!\~english	Name of the render size (xy) and material index (z) frame variable.
		//!\~french		Nom de la frame variable contenant la taille de rendu (xy) et l'index du matériau (z).
		C3D_API static castor::String const RenderSizeIndex;
	};
}

#define UBO_OVERLAY( writer, binding, set )\
	sdw::Ubo overlay{ writer, OverlayUbo::BufferOverlayName, binding, set };\
	auto c3d_positionRatio = overlay.declMember< sdw::Vec4 >( castor3d::OverlayUbo::PositionRatio );\
	auto c3d_renderSizeIndex = overlay.declMember< sdw::IVec4 >( castor3d::OverlayUbo::RenderSizeIndex );\
	overlay.end()

#endif
