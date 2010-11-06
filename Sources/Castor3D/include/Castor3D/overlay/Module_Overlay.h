#ifndef ___Module_Overlay___
#define ___Module_Overlay___

#include "../Module_General.h"

namespace Castor3D
{
	class Overlay;
	class PanelOverlay;
	class BorderPanelOverlay;
	class TextOverlay;
	class OverlayManager;
	
	typedef Templates::SharedPtr<Overlay>				OverlayPtr;
	typedef Templates::SharedPtr<PanelOverlay>			PanelOverlayPtr;
	typedef Templates::SharedPtr<BorderPanelOverlay>	BorderPanelOverlayPtr;
	typedef Templates::SharedPtr<TextOverlay>			TextOverlayPtr;

	typedef C3DMap( int,	OverlayPtr)	OverlayPtrIntMap;
	typedef C3DMap( String, OverlayPtr)	OverlayPtrStrMap;
}

#endif