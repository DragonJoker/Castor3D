#include "Castor3D/Overlay/Overlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

CU_ImplementCUSmartPtr( castor3d, Overlay )

namespace castor3d
{
	Overlay::Overlay( castor::String const & name
		, Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent )
		: OwnedBy< Engine >{ engine }
		, m_name{ name }
		, m_parent{ parent }
		, m_category{ engine.getOverlayCache().getFactory().create( type ) }
		, m_scene{ scene }
		, m_renderSystem{ engine.getRenderSystem() }
	{
		m_category->setOverlay( this );
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent )
		: Overlay{ castor::String{}
			, engine
			, type
			, scene
			, parent }
	{
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type )
		: Overlay{ engine
			, type
			, nullptr
			, nullptr }
	{
	}

	void Overlay::addChild( OverlayRPtr overlay )
	{
		int index = 1;

		if ( !m_overlays.empty() )
		{
			index = ( *( m_overlays.end() - 1 ) )->getIndex() + 1;
		}

		overlay->setOrder( index, getLevel() + 1 );
		m_overlays.push_back( overlay );
	}

	uint32_t Overlay::getChildrenCount( int level )const
	{
		uint32_t result{ 0 };

		if ( level == getLevel() + 1 )
		{
			result = uint32_t( m_overlays.size() );
		}
		else if ( level > getLevel() )
		{
			for ( auto overlay : m_overlays )
			{
				result += overlay->getChildrenCount( level );
			}
		}

		return result;
	}

	PanelOverlaySPtr Overlay::getPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::ePanel )
		{
			CU_Exception( "This overlay is not a panel." );
		}

		return std::static_pointer_cast< PanelOverlay >( m_category );
	}

	BorderPanelOverlaySPtr Overlay::getBorderPanelOverlay()const
	{
		if ( m_category->getType() != OverlayType::eBorderPanel )
		{
			CU_Exception( "This overlay is not a border panel." );
		}

		return std::static_pointer_cast< BorderPanelOverlay >( m_category );
	}

	TextOverlaySPtr Overlay::getTextOverlay()const
	{
		if ( m_category->getType() != OverlayType::eText )
		{
			CU_Exception( "This overlay is not a text." );
		}

		return std::static_pointer_cast< TextOverlay >( m_category );
	}

	bool Overlay::isVisible()const
	{
		bool result = m_category->isVisible();

		if ( result && getParent() )
		{
			result = getParent()->isVisible();
		}

		return result;
	}

	uint32_t Overlay::computeLevel()const
	{
		if ( !m_parent )
		{
			return 0u;
		}

		return 1u + m_parent->computeLevel();
	}
}
