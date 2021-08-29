#include "Castor3D/Miscellaneous/ProgressBar.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/Cache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

CU_ImplementCUSmartPtr( castor3d, ProgressBar )

namespace castor3d
{
	//*********************************************************************************************

	ProgressBar::ProgressBar( Engine & engine
		, OverlaySPtr parent
		, OverlaySPtr bar
		, TextOverlaySPtr title
		, TextOverlaySPtr label
		, uint32_t max )
		: m_engine{ engine }
		, m_listener{ engine.getFrameListenerCache().add( "C3D_ProgressBar" ) }
		, m_index{ castor::makeRangedValue( 0u, 0u, max ) }
		, m_progress{ parent }
		, m_progressBar{ bar }
		, m_progressTitle{ title }
		, m_progressLabel{ label }
	{
	}

	void ProgressBar::setTitle( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_title = value;
		m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, value]()
			{
				if ( m_progressLabel )
				{
					m_progressTitle->setCaption( m_title );
				}
			} ) );
	}

	void ProgressBar::setLabel( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, value]()
			{
				if ( m_progressLabel )
				{
					m_progressLabel->setCaption( value );
				}
			} ) );
	}

	void ProgressBar::step( castor::String const & label )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_index = m_index + 1u;
		auto index = m_index.percent();
		m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, label, index]()
			{
				if ( m_progressLabel )
				{
					m_progressLabel->setCaption( label );
				}

				if ( m_progressBar )
				{
					m_progressBar->setSize( { index, 1.0f } );
				}
			} ) );
	}

	void ProgressBar::step()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_index = m_index + 1u;
		auto index = m_index.percent();
		m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this, index]()
			{
				if ( m_progressBar )
				{
					m_progressBar->setSize( { index, 1.0f } );
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
		, uint32_t value )
	{
		if ( progress )
		{
			progress->incRange( value );
		}
	}

	//*********************************************************************************************
}
