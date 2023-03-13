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
		, LayoutCtrlFlags flags )
	{
		auto it = std::find_if( m_controls.begin()
			, m_controls.end()
			, [&control]( LayoutCtrl const & lookup )
			{
				return lookup.control() == &control;
			} );

		if ( it != m_controls.end() )
		{
			CU_Exception( "The control already exists in the layout." );
		}

		m_controls.emplace_back( control, std::move( flags ) );
		markDirty();
	}

	void Layout::addSpacer( uint32_t size
		, LayoutCtrlFlags flags )
	{
		auto & spacer = m_spacers.emplace_back( castor::makeUnique< Spacer >( size ) );
		m_controls.emplace_back( *spacer, std::move( flags ) );
		markDirty();
	}

	void Layout::update()
	{
		doUpdate();
	}
}
