#include "Castor3D/Gui/Layout/Layout.hpp"

#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

CU_ImplementSmartPtr( castor3d, Layout )
CU_ImplementSmartPtr( castor3d, Spacer )

namespace castor3d
{
	//*************************************************************************

	castor::Size Layout::Item::getPaddedSize()const noexcept
	{
		castor::Size result;

		if ( isControl() )
		{
			result = control()->getSize();
			result->x += m_flags.paddingSize( 0u );
			result->y += m_flags.paddingSize( 1u );
		}

		return result;
	}

	//*************************************************************************

	Layout::Layout( castor::String const & typeName
		, LayoutControl & container )
		: castor::Named{ typeName }
		, m_container{ container }
	{
	}

	Layout::~Layout()noexcept
	{
		if ( m_event )
		{
			m_event->skip();
			m_event = nullptr;
		}
	}

	void Layout::markDirty()
	{
		if ( !m_event )
		{
			m_event = m_container.getEngine().postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					m_event = nullptr;
					update();
				} ) );
		}
	}

	void Layout::removeControl( Control & control )
	{
		auto it = std::find_if( m_items.begin()
			, m_items.end()
			, [&control]( Item const & lookup )
			{
				return lookup.control() == &control;
			} );

		if ( it == m_items.end() )
		{
			log::warn << "Layout: The control [" << control.getName() << "] was not found in the layout." << std::endl;
			return;
		}

		m_items.erase( it );
		markDirty();
	}

	void Layout::addControl( Control & control
		, LayoutItemFlags flags )
	{
		auto it = std::find_if( m_items.begin()
			, m_items.end()
			, [&control]( Item const & lookup )
			{
				return lookup.control() == &control;
			} );

		if ( it != m_items.end() )
		{
			CU_SrcException( "Layout", "The control already exists in the layout." );
		}

		// TODO: Allow dragging elements in a layout.
		control.removeFlag( ControlFlag::eMovable );

		m_items.emplace_back( control
			, std::move( flags )
			, control.onChanged.connect( [this]( Control const & ctrl )
				{
					if ( !m_updating )
					{
						markDirty();
					}
				} ) );
		markDirty();
	}

	void Layout::addSpacer( uint32_t size )
	{
		auto & spacer = m_spacers.emplace_back( castor::makeUnique< Spacer >( size ) );
		m_items.emplace_back( *spacer );
		markDirty();
	}

	void Layout::update()
	{
		if ( !m_updating.exchange( true ) )
		{
			doUpdate();
			m_updating = false;
		}
	}
}
