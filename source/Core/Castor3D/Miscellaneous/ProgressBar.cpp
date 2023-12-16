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

	void ProgressBar::ProgressLabel::update( ProgressCtrlRPtr value )
	{
		progress = value;
		doSetTitle( castor::string::toU32String( title ) );
		doSetLabel( castor::string::toU32String( label ) );
	}

	void ProgressBar::ProgressLabel::initRange( int32_t mod )
	{
		if ( progress )
		{
			setRange( mod );
		}
	}

	void ProgressBar::ProgressLabel::setTitle( castor::String const & value )
	{
		title = value;
		doSetTitle( castor::string::toU32String( title ) );
	}

	void ProgressBar::ProgressLabel::setLabel( castor::String const & value )
	{
		label = value;
		doSetLabel( castor::string::toU32String( label ) );
	}

	void ProgressBar::ProgressLabel::step( castor::String const & value )
	{
		doStep();
		label = value;
		doSetLabel( castor::string::toU32String( label ) );
	}

	void ProgressBar::ProgressLabel::step()
	{
		doStep();
	}

	void ProgressBar::ProgressLabel::setStep( castor::String const & text, int32_t value )
	{
		listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this, value]()
			{
				if ( progress )
				{
					progress->setProgress( value );
				}
			} ) );
		label = text;
		doSetLabel( castor::string::toU32String( label ) );
	}

	void ProgressBar::ProgressLabel::setRange( int32_t value )
	{
		if ( rangeEvent )
		{
			rangeEvent->skip();
			rangeEvent = nullptr;
		}

		rangeEvent = listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this, value]()
			{
				rangeEvent = nullptr;

				if ( progress )
				{
					progress->setRange( castor::makeRange( 0, value ) );
				}
			} ) );
	}

	void ProgressBar::ProgressLabel::incRange( int32_t mod )
	{
		if ( progress )
		{
			setRange( progress->getRange().getMax() + mod );
		}
	}

	int32_t ProgressBar::ProgressLabel::getIndex()const
	{
		if ( progress )
		{
			return progress->getProgress();
		}

		return 0;
	}

	void ProgressBar::ProgressLabel::doSetTitle( castor::U32String const & value )
	{
		if ( titleEvent )
		{
			titleEvent->skip();
			titleEvent = nullptr;
		}

		titleEvent = listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this, value]()
			{
				titleEvent = nullptr;

				if ( progress )
				{
					progress->setTitle( value );
				}
			} ) );
	}

	void ProgressBar::ProgressLabel::doSetLabel( castor::U32String const & value )
	{
		if ( labelEvent )
		{
			labelEvent->skip();
			labelEvent = nullptr;
		}

		labelEvent = listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this, value]()
			{
				labelEvent = nullptr;

				if ( progress )
				{
					progress->setCaption( value );
				}
			} ) );
	}

	void ProgressBar::ProgressLabel::doStep()
	{
		listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this]()
			{
				if ( progress )
				{
					progress->incProgress();
				}
			} ) );
	}

	//*********************************************************************************************

	ProgressBar::ProgressBar( Engine & engine
		, ProgressCtrlRPtr globalProgress
		, ProgressCtrlRPtr localProgress )
		: m_listener{ engine.addNewFrameListener( "C3D_ProgressBar" ) }
		, m_global{ m_listener, globalProgress }
		, m_local{ m_listener, localProgress }
	{
	}

	void ProgressBar::update( ProgressCtrlRPtr globalProgress
		, ProgressCtrlRPtr localProgress )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.update( globalProgress );
		m_local.update( localProgress );
	}

	void ProgressBar::initGlobalRange( uint32_t value )
	{
		m_global.initRange( int32_t( value ) );
	}

	void ProgressBar::stepGlobal( castor::String const & globalTitle )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.setTitle( globalTitle );
		m_global.step();
	}

	void ProgressBar::initLocalRange( castor::String const & globalLabel
		, uint32_t value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.setLabel( globalLabel );
		m_local.initRange( int32_t( value ) );
	}

	void ProgressBar::stepLocal( castor::String const & localLabel )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.step( localLabel );
	}

	void ProgressBar::setLocalStep( castor::String const & label, uint32_t count )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.setStep( label, int32_t( count ) );
	}

	//*********************************************************************************************

	void initProgressBarGlobalRange( ProgressBar * progress
		, uint32_t value )
	{
		if ( progress )
		{
			progress->initGlobalRange( value );
		}
	}

	void stepProgressBarGlobal( ProgressBar * progress
		, castor::String const & globalTitle )
	{
		if ( progress )
		{
			progress->stepGlobal( globalTitle );
		}
	}

	void initProgressBarLocalRange( ProgressBar * progress
		, castor::String const & globalLabel
		, uint32_t value )
	{
		if ( progress )
		{
			progress->initLocalRange( globalLabel, value );
		}
	}

	void stepProgressBarLocal( ProgressBar * progress
		, castor::String const & localLabel )
	{
		if ( progress )
		{
			progress->stepLocal( localLabel );
		}
	}

	void setProgressBarLocalStep( ProgressBar * progress
		, castor::String const & localLabel
		, uint32_t count )
	{
		if ( progress )
		{
			progress->setLocalStep( localLabel, count );
		}
	}

	//*********************************************************************************************
}
