#include "Castor3D/Gui/Controls/CtrlFrame.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, FrameCtrl )

namespace castor3d
{
	FrameCtrl::FrameCtrl( SceneRPtr scene
		, castor::String const & name
		, FrameStyle * style
		, ControlRPtr parent )
		: FrameCtrl{ scene
			, name
			, style
			, parent
			, castor::Position{}
			, castor::Size{}
			, 25u
			, true
			, 0u
			, true }
	{
	}

	FrameCtrl::FrameCtrl( SceneRPtr scene
		, castor::String const & name
		, FrameStyle * style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t headerHeight
		, bool expanded
		, ControlFlagType flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags | ControlFlag::eMovable | ControlFlag::eResizable
			, visible }
		, m_headerHeight{ headerHeight }
		, m_header{ std::make_shared< StaticCtrl >( m_scene
			, cuT( "Header" )
			, &style->getHeaderStyle()
			, this
			, castor::string::toU32String( name )
			, castor::Position{ 0, 0 }
			, castor::Size{ size->x - m_headerHeight, m_headerHeight } ) }
		, m_content{ std::make_shared< PanelCtrl >( m_scene
			, cuT( "Content" )
			, &style->getContentStyle()
			, this
			, castor::Position{ 0, int32_t( m_headerHeight ) }
			, castor::Size{ size->x, size->y - m_headerHeight } ) }
	{
		setBorderSize( { 0u, 0u, 0u, 0u } );
		m_header->setVisible( visible );
		m_header->setBorderSize( { 0u, 0u, 0u, 1u } );
		m_content->setVisible( visible );
		setStyle( style );
	}

	void FrameCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_header->setStyle( &style.getHeaderStyle() );
		m_content->setStyle( &style.getContentStyle() );
	}

	void FrameCtrl::doCreate()
	{
		doUpdatePositions();
		doUpdateSizes();

		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.create( m_header );
		manager.create( m_content );
	}

	void FrameCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();

		if ( m_header )
		{
			manager.destroy( m_header );
		}

		if ( m_content )
		{
			manager.destroy( m_content );
		}
	}

	castor::Position FrameCtrl::doUpdatePosition( castor::Position const & value )const noexcept
	{
		return { std::max( 0, value.x() )
			, std::max( 0, value.y() ) };
	}

	castor::Size FrameCtrl::doUpdateSize( castor::Size const & value )const noexcept
	{
		return { std::max( m_minSize->x, value->x )
			, std::max( m_minSize->y, value->y ) };
	}

	void FrameCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdatePositions();
	}

	void FrameCtrl::doSetSize( castor::Size const & value )
	{
		doUpdatePositions();
		doUpdateSizes();
	}

	void FrameCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		doUpdatePositions();
		doUpdateSizes();
	}

	bool FrameCtrl::doCatchesMouseEvents()const
	{
		return true;
	}

	void FrameCtrl::doSetVisible( bool visible )
	{
		m_header->setVisible( visible );
		m_content->setVisible( visible );
	}

	void FrameCtrl::doUpdatePositions()
	{
		auto clientOffset = getClientOffset();
		auto headerHeight = int32_t( m_header->getSize()->y );
		m_header->setPosition( clientOffset );
		m_content->setPosition( { clientOffset.x(), clientOffset.y() + headerHeight } );
	}

	void FrameCtrl::doUpdateSizes()
	{
		auto clientSize = getClientSize();
		auto headerHeight = m_header->getSize()->y;
		m_header->setSize( { clientSize->x, headerHeight } );
		m_content->setSize( { clientSize->x, clientSize->y - headerHeight } );
	}
}
