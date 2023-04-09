#include "Castor3D/Miscellaneous/ProgressBar.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Gui/Controls/CtrlProgress.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementSmartPtr( castor3d, ProgressBar )

namespace castor3d
{
	//*********************************************************************************************

	ProgressBar::ProgressBar( Engine & engine
		, ProgressCtrlRPtr progress )
		: m_listener{ engine.addNewFrameListener( "C3D_ProgressBar" ) }
		, m_progress{ progress }
	{
	}

	void ProgressBar::update( ProgressCtrlRPtr progress )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_progress = progress;
		doSetTitle( castor::string::toU32String( m_title ) );
		doSetLabel( castor::string::toU32String( m_label ) );
	}

	void ProgressBar::setTitle( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_title = value;
		doSetTitle( castor::string::toU32String( m_title ) );
	}

	void ProgressBar::setLabel( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_label = value;
		doSetLabel( castor::string::toU32String( m_label ) );
	}

	void ProgressBar::step( castor::String const & label )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doStep();
		m_label = label;
		doSetLabel( castor::string::toU32String( m_label ) );
	}

	void ProgressBar::step()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doStep();
	}

	void ProgressBar::setRange( int32_t value )
	{
		if ( m_rangeEvent )
		{
			m_rangeEvent->skip();
			m_rangeEvent = nullptr;
		}

		m_rangeEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, value]()
			{
				m_rangeEvent = nullptr;

				if ( m_progress )
				{
					m_progress->setRange( castor::makeRange( 0, value ) );
				}
			} ) );
	}

	void ProgressBar::incRange( int32_t mod )
	{
		if ( m_progress )
		{
			setRange( m_progress->getRange().getMax() + mod );
		}
	}

	int32_t ProgressBar::getIndex()const
	{
		if ( m_progress )
		{
			return m_progress->getProgress();
		}

		return 0;
	}

	void ProgressBar::doSetTitle( castor::U32String const & value )
	{
		if ( m_titleEvent )
		{
			m_titleEvent->skip();
			m_titleEvent = nullptr;
		}

		m_titleEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, value]()
			{
				m_titleEvent = nullptr;

				if ( m_progress )
				{
					m_progress->setTitle( value );
				}
			} ) );
	}

	void ProgressBar::doSetLabel( castor::U32String const & value )
	{
		if ( m_labelEvent )
		{
			m_labelEvent->skip();
			m_labelEvent = nullptr;
		}

		m_labelEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, value]()
			{
				m_labelEvent = nullptr;

				if ( m_progress )
				{
					m_progress->setCaption( value );
				}
			} ) );
	}

	void ProgressBar::doStep()
	{
		m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				if ( m_progress )
				{
					m_progress->incProgress();
				}
			} ) );
	}

	//*********************************************************************************************

	void setProgressBarTitle( ProgressBar * progress
		, castor::String const & value )
	{
		if ( progress )
		{
			progress->setTitle( value );
		}
	}

	void setProgressBarLabel( ProgressBar * progress
		, castor::String const & value )
	{
		if ( progress )
		{
			progress->setLabel( value );
		}
	}

	void stepProgressBar( ProgressBar * progress
		, castor::String const & label )
	{
		if ( progress )
		{
			progress->step( label );
		}
	}

	void stepProgressBar( ProgressBar * progress )
	{
		if ( progress )
		{
			progress->step();
		}
	}

	void incProgressBarRange( ProgressBar * progress
		, int32_t value )
	{
		if ( progress )
		{
			progress->incRange( value );
		}
	}

	//*********************************************************************************************
}
