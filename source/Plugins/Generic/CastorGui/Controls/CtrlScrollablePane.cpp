#include "CastorGui/Controls/CtrlScrollablePane.hpp"

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
	ScrollablePaneCtrl::ScrollablePaneCtrl( String const & name
		, Engine & engine
		, ControlRPtr parent
		, uint32_t id )
		: ScrollablePaneCtrl( name
			, engine
			, parent
			, id
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	ScrollablePaneCtrl::ScrollablePaneCtrl( String const & name
		, Engine & engine
		, ControlRPtr parent
		, uint32_t id
		, Position const & position
		, Size const & size
		, uint32_t flags
		, bool visible )
		: Control( ControlType::eScrollablePane
			, name
			, engine
			, parent
			, id
			, position
			, size
			, flags
			, visible )
	{
		setBackgroundBorders( castor::Rectangle() );
	}

	void ScrollablePaneCtrl::doCreate()
	{
		if ( !m_foregroundMaterial )
		{
			m_foregroundMaterial = createMaterial( getEngine()
				, cuT( "CtrlScrollablePane_FG_" ) + string::toString( getId() )
				, RgbColour::fromComponents( 1.0, 1.0, 1.0 ) );
		}
	}

	void ScrollablePaneCtrl::doDestroy()
	{
	}

	void ScrollablePaneCtrl::doSetPosition( Position const & value )
	{
	}

	void ScrollablePaneCtrl::doSetSize( Size const & value )
	{
	}

	void ScrollablePaneCtrl::doSetBackgroundMaterial( MaterialRPtr material )
	{
	}

	void ScrollablePaneCtrl::doSetForegroundMaterial( MaterialRPtr material )
	{
	}

	void ScrollablePaneCtrl::doSetVisible( bool visible )
	{
	}

	void ScrollablePaneCtrl::doUpdateFlags()
	{
	}
}
