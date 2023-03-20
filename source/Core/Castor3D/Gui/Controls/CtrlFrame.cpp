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
			, name
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

	void FrameCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdatePositions();
	}

	void FrameCtrl::doSetSize( castor::Size const & value )
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
		auto maxHeight = m_header->getSize()->y;
		auto & borders = getBorderSize();
		m_header->setPosition( castor::Position( int32_t( borders->x ), int32_t( borders->y ) ) );
		m_content->setPosition( castor::Position( 0, int32_t( borders->y + maxHeight ) ) );
	}

	void FrameCtrl::doUpdateSizes()
	{
		auto & size = getSize();
		auto & borders = getBorderSize();
		auto headerSize = m_header->getSize()->y;
		auto bordersWidth = ( borders->x + borders->z );
		m_header->setSize( castor::Size( size->x - bordersWidth, headerSize ) );
		m_content->setSize( castor::Size( size->x, size->y - ( headerSize + borders->y ) ) );
	}
}
