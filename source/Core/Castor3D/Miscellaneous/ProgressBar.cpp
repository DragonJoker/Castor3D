#include "Castor3D/Miscellaneous/ProgressBar.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementCUSmartPtr( castor3d, ProgressBar )

namespace castor3d
{
	//*********************************************************************************************

	ProgressBar::ProgressBar( Engine & engine
		, OverlayResPtr parent
		, OverlayResPtr bar
		, TextOverlaySPtr title
		, TextOverlaySPtr label
		, uint32_t max )
		: m_listener{ engine.addNewFrameListener( "C3D_ProgressBar" ).lock() }
		, m_index{ castor::makeRangedValue( 0u, 0u, max ) }
		, m_progress{ parent.lock().get() }
		, m_progressBar{ bar.lock().get() }
		, m_progressTitle{ title }
		, m_progressLabel{ label }
	{
	}

	void ProgressBar::update( OverlayResPtr parent
		, OverlayResPtr bar
		, TextOverlaySPtr title
		, TextOverlaySPtr label )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		if ( m_titleEvent )
		{
			m_titleEvent->skip();
			m_titleEvent = nullptr;
		}

		if ( m_labelEvent )
		{
			m_labelEvent->skip();
			m_labelEvent = nullptr;
		}

		if ( m_stepLabelEvent )
		{
			m_stepLabelEvent->skip();
			m_stepLabelEvent = nullptr;
		}

		if ( m_stepEvent )
		{
			m_stepEvent->skip();
			m_stepEvent = nullptr;
		}

		m_progress = parent.lock().get();
		m_progressBar = bar.lock().get();
		m_progressTitle = title;
		m_progressLabel = label;
		m_titleEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				if ( m_progressTitle )
				{
					m_progressTitle->setCaption( m_title );
				}

				m_titleEvent = nullptr;
			} ) );
		auto index = m_index.percent();
		m_stepLabelEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, index]()
			{
				if ( m_progressLabel )
				{
					m_progressLabel->setCaption( m_label );
				}

				if ( m_progressBar )
				{
					m_progressBar->setSize( { index, 1.0f } );
				}

				m_stepLabelEvent = nullptr;
			} ) );
	}

	void ProgressBar::setTitle( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_title = value;

		if ( m_titleEvent )
		{
			m_titleEvent->skip();
		}

		m_titleEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				if ( m_progressTitle )
				{
					m_progressTitle->setCaption( m_title );
				}

				m_titleEvent = nullptr;
			} ) );
	}

	void ProgressBar::setLabel( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_label = value;

		if ( m_labelEvent )
		{
			m_labelEvent->skip();
		}

		m_labelEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				if ( m_progressLabel )
				{
					m_progressLabel->setCaption( m_label );
				}

				m_labelEvent = nullptr;
			} ) );
	}

	void ProgressBar::step( castor::String const & label )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_label = label;
		m_index = m_index + 1u;
		auto index = m_index.percent();

		if ( m_stepLabelEvent )
		{
			m_stepLabelEvent->skip();
		}

		m_stepLabelEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, index]()
			{
				if ( m_progressLabel )
				{
					m_progressLabel->setCaption( m_label );
				}

				if ( m_progressBar )
				{
					m_progressBar->setSize( { index, 1.0f } );
				}

				m_stepLabelEvent = nullptr;
			} ) );
	}

	void ProgressBar::step()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_index = m_index + 1u;
		auto index = m_index.percent();

		if ( m_stepEvent )
		{
			m_stepEvent->skip();
		}

		m_stepEvent = m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, index]()
			{
				if ( m_progressBar )
				{
					m_progressBar->setSize( { index, 1.0f } );
				}

				m_stepEvent = nullptr;
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
		, uint32_t value )
	{
		if ( progress )
		{
			progress->incRange( value );
		}
	}

	//*********************************************************************************************
}
