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

		static I3DControllerUPtr create( c3d::String const & appName
			, c3d::FrameListener & listener );

		virtual void initialise() = 0;
		virtual void cleanup() = 0;

		virtual void reset() = 0;
		virtual void setCamera( c3d::CameraRPtr camera ) = 0;
		virtual void setGeometry( c3d::Geometry const * geometry ) = 0;

		virtual void setSpeedFactor( float value ) = 0;
		virtual void multSpeedFactor( float value ) = 0;

		virtual void invertXTranslate( bool invert ) = 0;
		virtual void invertYTranslate( bool invert ) = 0;
		virtual void invertZTranslate( bool invert ) = 0;
	};
}

#endif
