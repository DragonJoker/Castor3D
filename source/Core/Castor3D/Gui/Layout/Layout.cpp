#include "Castor3D/Gui/Layout/Layout.hpp"

#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Gui/Controls/CtrlLayoutControl.hpp"

CU_ImplementCUSmartPtr( castor3d, Layout )
CU_ImplementCUSmartPtr( castor3d, Spacer )

namespace castor3d
{
	Layout::Layout( castor::String const & typeName
		, LayoutControl & container )
		: castor::Named{ typeName }
		, m_container{ container }
	{
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
