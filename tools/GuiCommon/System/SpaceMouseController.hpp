/*
See LICENSE file in root folder
*/
#ifndef ___GC_SpaceMouseController_H___
#define ___GC_SpaceMouseController_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

namespace GuiCommon
{
	class I3DController
	{
	public:
		virtual ~I3DController()noexcept = default;

		static I3DControllerUPtr create( castor::String const & appName
			, castor3d::FrameListener & listener );

		virtual void reset() = 0;
		virtual void setCamera( castor3d::CameraRPtr camera ) = 0;
		virtual void setGeometry( castor3d::GeometryRPtr geometry ) = 0;

		virtual void setSpeedFactor( float value ) = 0;
		virtual void multSpeedFactor( float value ) = 0;

		virtual void invertXTranslate( bool invert ) = 0;
		virtual void invertYTranslate( bool invert ) = 0;
		virtual void invertZTranslate( bool invert ) = 0;
	};
}

#endif
