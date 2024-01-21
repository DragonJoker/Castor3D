#include "Castor3D/Overlay/Overlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

CU_ImplementSmartPtr( castor3d, Overlay )
CU_ImplementSmartPtr( castor3d, OverlayContext )

namespace castor3d
{
	Overlay::Overlay( castor::String const & name
		, Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent
		, uint32_t level )
		: OwnedBy< Engine >{ engine }
		, m_name{ name }
		, m_parent{ parent }
		, m_category{ scene
			? scene->getOverlayCache().getFactory().create( type )
			: engine.getOverlayCache().getFactory().create( type ) }
		, m_scene{ scene }
		, m_renderSystem{ engine.getRenderSystem() }
	{
		m_category->setOverlay( this );
		m_category->setOrder( level, 0u );
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type
		, SceneRPtr scene
		, OverlayRPtr parent
		, uint32_t level )
		: Overlay{ castor::String{}
			, engine
			, type
			, scene
			, parent
			, level }
	{
	}

	Overlay::Overlay( Engine & engine
		, OverlayType type
		, uint32_t level )
		: Overlay{ engine
			, type
			, nullptr
			, nullptr
			, level }
	{
	}

	uint32_t Overlay::getChildrenCount( uint32_t level )const
	{
		uint32_t result{ 0 };

		if ( level == getLevel() )
		{
			result = 1u;
		}
		else if ( level == getLevel() + 1u )
		{
			result = uint32_t( m_children.size() );
		}
		else if ( level > getLevel() )
		{
			for ( auto overlay : m_children )
			{
				result += overlay->getChildrenCount( level );
			}
		}

		return result;
	}

	PanelOverlayRPtr Overlay::getPanelOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::ePanel )
		{
			CU_Failure( "This overlay is not a panel." );
			return nullptr;
		}

		return &static_cast< PanelOverlay & >( *m_category );
	}

	BorderPanelOverlayRPtr Overlay::getBorderPanelOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::eBorderPanel )
		{
			CU_Failure( "This overlay is not a border panel." );
			return nullptr;
		}

		return &static_cast< BorderPanelOverlay & >( *m_category );
	}

	TextOverlayRPtr Overlay::getTextOverlay()const noexcept
	{
		if ( m_category->getType() != OverlayType::eText )
		{
			CU_Failure( "This overlay is not a text." );
			return nullptr;
		}

		return &static_cast< TextOverlay & >( *m_category );
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

	bool Overlay::isDisplayable()const
	{
		bool result = m_category->isDisplayable();

		if ( result && getParent() )
		{
			result = getParent()->isDisplayable();
		}

		return result;
	}

	uint32_t Overlay::computeLevel()const
	{
		if ( !m_parent )
		{
			return m_category->getLevel();
		}

		return 1u + m_parent->computeLevel();
	}

	void Overlay::addChild( OverlayRPtr overlay )
	{
		m_children.push_back( overlay );
		overlay->m_parent = this;
	}

	void Overlay::removeChild( OverlayRPtr overlay )
	{
		if ( auto it = std::find( m_children.begin()
				, m_children.end()
				, overlay );
			it != m_children.end() )
		{
			( *it )->m_parent = nullptr;
			m_children.erase( it );
		}
	}

	void Overlay::clear()noexcept
	{
		for ( auto child : m_children )
		{
			child->m_parent = nullptr;
		}

		m_children.clear();
	}

	Engine * getEngine( OverlayContext const & context )
	{
		return getEngine( *context.root );
	}
}
