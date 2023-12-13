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
		m_global.initRange( value );
	}

	void ProgressBar::setGlobalTitle( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.setTitle( value );
	}

	void ProgressBar::setGlobalLabel( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.setLabel( value );
	}

	void ProgressBar::stepGlobal( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.step( value );
	}

	void ProgressBar::stepGlobal()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_global.step();
	}

	void ProgressBar::setGlobalRange( int32_t value )
	{
		m_global.setRange( value );
	}

	void ProgressBar::incGlobalRange( int32_t value )
	{
		m_global.incRange( value );
	}

	int32_t ProgressBar::getGlobalIndex()const
	{
		return m_global.getIndex();
	}

	void ProgressBar::setLocalTitle( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.setTitle( value );
	}

	void ProgressBar::setLocalLabel( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.setLabel( value );
	}

	void ProgressBar::stepLocal( castor::String const & value )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.step( value );
	}

	void ProgressBar::stepLocal()
	{
		auto lock( castor::makeUniqueLock( *this ) );
		m_local.step();
	}

	void ProgressBar::setLocalRange( int32_t value )
	{
		m_local.setRange( value );
	}

	void ProgressBar::incLocalRange( int32_t value )
	{
		m_local.incRange( value );
	}

	int32_t ProgressBar::getLocalIndex()const
	{
		return m_local.getIndex();
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

	void setProgressBarGlobalLabel( ProgressBar * progress
		, castor::String const & value )
	{
		if ( progress )
		{
			progress->setGlobalLabel( value );
		}
	}

	void stepProgressBarGlobal( ProgressBar * progress
		, castor::String const & label )
	{
		if ( progress )
		{
			progress->stepGlobal( label );
		}
	}

	void stepProgressBarGlobal( ProgressBar * progress )
	{
		if ( progress )
		{
			progress->stepGlobal();
		}
	}

	void setProgressBarLocalTitle( ProgressBar * progress
		, castor::String const & value )
	{
		if ( progress )
		{
			progress->setLocalTitle( value );
		}
	}

	void setProgressBarLocalLabel( ProgressBar * progress
		, castor::String const & value )
	{
		if ( progress )
		{
			progress->setLocalLabel( value );
		}
	}

	void stepProgressBarLocal( ProgressBar * progress
		, castor::String const & label )
	{
		if ( progress )
		{
			progress->stepLocal( label );
		}
	}

	void stepProgressBarLocal( ProgressBar * progress )
	{
		if ( progress )
		{
			progress->stepLocal();
		}
	}

	void incProgressBarLocalRange( ProgressBar * progress
		, int32_t value )
	{
		if ( progress )
		{
			progress->incLocalRange( value );
		}
	}

	//*********************************************************************************************
}
