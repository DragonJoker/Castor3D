#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( c3d, ExpandablePanelCtrl )

namespace c3d
{
	ExpandablePanelCtrl::ExpandablePanelCtrl( SceneRPtr scene
		, String const & name
		, ExpandablePanelStyle * style
		, ControlRPtr parent )
		: ExpandablePanelCtrl{ scene
			, name
			, style
			, parent
			, Position{}
			, Size{}
			, 25u
			, true
			, 0u
			, true }
	{
	}

	ExpandablePanelCtrl::ExpandablePanelCtrl( SceneRPtr scene
		, String const & name
		, ExpandablePanelStyle * style
		, ControlRPtr parent
		, Position const & position
		, Size const & size
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
			, flags
			, visible }
		, m_headerHeight{ headerHeight }
		, m_header{ getEngine().getControlsManager()->registerControlT( makeUnique< PanelCtrl >( m_scene
			, cuT( "Header" )
			, &style->getHeaderStyle()
			, this
			, Position{ 0, 0 }
			, Size{ size->x - m_headerHeight, m_headerHeight } ) ) }
		, m_expand{ getEngine().getControlsManager()->registerControlT( makeUnique< ButtonCtrl >( m_scene
			, cuT( "Expand" )
			, &style->getExpandStyle()
			, this
			, m_retractCaption
			, Position{ int32_t( size->x - m_headerHeight ), 0 }
			, Size{ m_headerHeight, m_headerHeight } ) ) }
		, m_content{ getEngine().getControlsManager()->registerControlT( makeUnique< PanelCtrl >( m_scene
			, cuT( "Content" )
			, &style->getContentStyle()
			, this
			, Position{ 0, int32_t( m_headerHeight ) }
			, Size{ size->x, size->y - m_headerHeight } ) ) }
		, m_expanded{ expanded }
	{
		setBorderSize( { 0u, 0u, 0u, 0u } );
		m_header->setVisible( visible );
		m_expand->setVisible( visible );
		m_content->setVisible( visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		setStyle( style );
	}

	ExpandablePanelCtrl::~ExpandablePanelCtrl()noexcept
	{
		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_content );
		manager.unregisterControl( *m_expand );
		manager.unregisterControl( *m_header );
	}

	void ExpandablePanelCtrl::setExpandCaption( U32String v )
	{
		m_expandCaption = c3d::move( v );
	}

	void ExpandablePanelCtrl::setRetractCaption( U32String v )
	{
		m_retractCaption = c3d::move( v );
	}

	void ExpandablePanelCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_header->setStyle( &style.getHeaderStyle() );
		m_expand->setStyle( &style.getExpandStyle() );
		m_content->setStyle( &style.getContentStyle() );
	}

	void ExpandablePanelCtrl::doCreate()
	{
		doUpdatePositions();
		doUpdateSizes();

		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.create( m_header );
		manager.create( m_expand );
		manager.create( m_content );

		manager.connectEvents( *this );

		if ( !m_expanded )
		{
			m_expanded = true;
			doSwitchExpand();
		}
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

		if ( m_content )
		{
			manager.destroy( m_content );
		}
	}

	void ExpandablePanelCtrl::doSetPosition( Position const & value )
	{
		doUpdatePositions();
	}

	void ExpandablePanelCtrl::doSetSize( Size const & value )
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
		m_content->setVisible( m_expanded && visible );
	}

	void ExpandablePanelCtrl::doSwitchExpand()
	{
		m_expanded = !m_expanded;
		m_content->setVisible( m_expanded && isVisible() );
		auto maxHeight = std::max( m_header->getSize()->y, m_expand->getSize()->y );
		auto size = getSize();
		size->y = maxHeight;

		if ( m_expanded )
		{
			size->y += m_content->getSize()->y;
		}

		setSize( size );

		if ( m_expanded )
		{
			m_expand->setCaption( m_retractCaption );
			m_signals[size_t( ExpandablePanelEvent::eExpand )]();
		}
		else
		{
			m_expand->setCaption( m_expandCaption );
			m_signals[size_t( ExpandablePanelEvent::eRetract )]();
		}
	}

	void ExpandablePanelCtrl::doUpdatePositions()
	{
		auto & size = getSize();
		auto expandSize = m_expand->getSize();
		auto maxHeight = std::max( m_header->getSize()->y, expandSize->y );
		m_header->setPosition( Position( 0, 0 ) );
		m_expand->setPosition( Position( int32_t( size->x - expandSize->x ), 0 ) );

		if ( m_expanded )
		{
			m_content->setPosition( Position( 0, int32_t( maxHeight ) ) );
		}
	}

	void ExpandablePanelCtrl::doUpdateSizes()
	{
		auto & size = getSize();
		auto expandSize = m_expand->getSize();
		auto headerSize = m_header->getSize();
		auto maxHeight = std::max( headerSize->y, expandSize->y );
		m_header->setSize( Size( size->x - expandSize->x, headerSize->y ) );
		m_expand->setSize( expandSize );

		if ( m_expanded )
		{
			m_content->setSize( Size( size->x, size->y - maxHeight ) );
		}
	}
}
