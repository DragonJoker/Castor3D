#include "CastorGui/Controls/CtrlFrame.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	FrameCtrl::FrameCtrl( String const & name
		, Engine & engine
		, ControlRPtr parent
		, uint32_t id )
		: FrameCtrl{ name
			, engine
			, parent
			, id
			, Position()
			, Size()
			, 0
			, true }
	{
	}

	FrameCtrl::FrameCtrl( String const & name
		, Engine & engine
		, ControlRPtr parent
		, uint32_t id
		, Position const & position
		, Size const & size
		, uint32_t flags
		, bool visible )
		: Control{ ControlType::eFrame
			, name
			, engine
			, parent
			, id
			, position
			, size
			, flags
			, visible }
	{
		setBackgroundBorders( castor::Rectangle() );
		doUpdateFlags();
	}

	void FrameCtrl::doCreate()
	{
		if ( !m_foregroundMaterial )
		{
			m_foregroundMaterial = createMaterial( getEngine(), cuT( "CtrlFrame_FG_" ) + string::toString( getId() ), RgbColour::fromComponents( 1.0, 1.0, 1.0 ) );
		}
	}

	void FrameCtrl::doDestroy()
	{
	}

	void FrameCtrl::doSetPosition( Position const & p_value )
	{
	}

	void FrameCtrl::doSetSize( Size const & p_value )
	{
	}

	void FrameCtrl::doSetBackgroundMaterial( MaterialRPtr p_material )
	{
	}

	void FrameCtrl::doSetForegroundMaterial( MaterialRPtr p_material )
	{
	}

	void FrameCtrl::doSetVisible( bool p_visible )
	{
	}

	void FrameCtrl::doUpdateFlags()
	{
	}
}
