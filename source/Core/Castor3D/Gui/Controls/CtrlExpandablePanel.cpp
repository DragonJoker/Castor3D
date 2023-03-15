#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, ExpandablePanelCtrl )

namespace castor3d
{
	ExpandablePanelCtrl::ExpandablePanelCtrl( SceneRPtr scene
		, castor::String const & name
		, ExpandablePanelStyle * style
		, ControlRPtr parent )
		: ExpandablePanelCtrl{ scene
			, name
			, style
			, parent
			, castor::Position{}
			, castor::Size{}
			, 25u
			, 0u
			, true }
	{
	}

	ExpandablePanelCtrl::ExpandablePanelCtrl( SceneRPtr scene
		, castor::String const & name
		, ExpandablePanelStyle * style
		, ControlRPtr parent
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t headerHeight
		, uint64_t flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
		, m_headerHeight{ headerHeight }
		, m_header{ std::make_shared< PanelCtrl >( m_scene
			, cuT( "Header" )
			, &style->getHeaderStyle()
			, this
			, castor::Position{ 0, 0 }
			, castor::Size{ size->x - m_headerHeight, m_headerHeight } ) }
		, m_expand{ std::make_shared< ButtonCtrl >( m_scene
			, cuT( "Expand" )
			, &style->getExpandStyle()
			, this
			, cuT( "-" )
			, castor::Position{ int32_t( size->x - m_headerHeight ), 0 }
			, castor::Size{ m_headerHeight, m_headerHeight } ) }
		, m_panel{ std::make_shared< PanelCtrl >( m_scene
			, cuT( "Panel" )
			, &style->getPanelStyle()
			, this
			, castor::Position{ 0, int32_t( m_headerHeight ) }
			, castor::Size{ size->x, size->y - m_headerHeight } ) }
	{
		setBackgroundBorderSize( { 0u, 0u, 0u, 0u } );
		m_header->setVisible( visible );
		m_expand->setVisible( visible );
		m_panel->setVisible( visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		doUpdateStyle();
	}

	void ExpandablePanelCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_header->setStyle( &style.getHeaderStyle() );
		m_expand->setStyle( &style.getExpandStyle() );
		m_panel->setStyle( &style.getPanelStyle() );
	}

	void ExpandablePanelCtrl::doCreate()
	{
		doUpdatePositions();
		doUpdateSizes();

		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.create( m_header );
		manager.create( m_expand );
		manager.create( m_panel );

		manager.connectEvents( *this );
	}

	void ExpandablePanelCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		getControlsManager()->disconnectEvents( *this );

		if ( m_expand )
		{
			manager.destroy( m_expand );
		}

		if ( m_header )
		{
			manager.destroy( m_header );
		}

		if ( m_panel )
		{
			manager.destroy( m_panel );
		}
	}

	void ExpandablePanelCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdatePositions();
	}

	void ExpandablePanelCtrl::doSetSize( castor::Size const & value )
	{
		doUpdatePositions();
		doUpdateSizes();
	}

	bool ExpandablePanelCtrl::doCatchesMouseEvents()const
	{
		return false;
	}

	void ExpandablePanelCtrl::doSetVisible( bool visible )
	{
		m_header->setVisible( visible );
		m_expand->setVisible( visible );
		m_panel->setVisible( m_expanded && visible );
	}

	void ExpandablePanelCtrl::doSwitchExpand()
	{
		m_expanded = !m_expanded;
		m_panel->setVisible( m_expanded && isVisible() );
		auto maxHeight = std::max( m_header->getSize()->y, m_expand->getSize()->y );
		auto size = getSize();
		size->y = maxHeight;

		if ( m_expanded )
		{
			size->y += m_panel->getSize()->y;
		}

		setSize( size );

		if ( m_expanded )
		{
			m_expand->setCaption( "-" );
			m_signals[size_t( ExpandablePanelEvent::eExpand )]();
		}
		else
		{
			m_expand->setCaption( "+" );
			m_signals[size_t( ExpandablePanelEvent::eRetract )]();
		}
	}

	void ExpandablePanelCtrl::doUpdatePositions()
	{
		auto & size = getSize();
		auto expandSize = m_expand->getSize();
		auto maxHeight = std::max( m_header->getSize()->y, expandSize->y );
		m_header->setPosition( castor::Position( 0, 0 ) );
		m_expand->setPosition( castor::Position( int32_t( size->x - expandSize->x ), 0 ) );

		if ( m_expanded )
		{
			m_panel->setPosition( castor::Position( 0, int32_t( maxHeight ) ) );
		}
	}

	void ExpandablePanelCtrl::doUpdateSizes()
	{
		auto & size = getSize();
		auto expandSize = m_expand->getSize();
		auto headerSize = m_header->getSize();
		auto maxHeight = std::max( headerSize->y, expandSize->y );
		m_header->setSize( castor::Size( size->x - expandSize->x, headerSize->y ) );
		m_expand->setSize( expandSize );

		if ( m_expanded )
		{
			m_panel->setSize( castor::Size( size->x, size->y - maxHeight ) );
		}
	}
}
