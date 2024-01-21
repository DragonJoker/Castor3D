#include "Castor3D/Miscellaneous/ProgressBar.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Gui/Controls/CtrlProgress.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/Debug.hpp>

CU_ImplementSmartPtr( castor3d, ProgressBar )

namespace castor3d
{
	//*********************************************************************************************

	void ProgressBar::ProgressLabel::update( ProgressCtrlRPtr ctrl )
	{
		progress = ctrl;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::initRange( int32_t newRangeMax )
	{
		data.value = 0;
		data.rangeMax = newRangeMax;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::setTitle( castor::StringView newTitle )
	{
		data.title = newTitle;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::setLabel( castor::StringView newLabel )
	{
		data.label = newLabel;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::step( castor::StringView newLabel )
	{
		data.label = newLabel;
		++data.value;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::step()
	{
		++data.value;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::setStep( castor::StringView newLabel, int32_t newValue )
	{
		data.label = newLabel;
		setStep( newValue );
	}

	void ProgressBar::ProgressLabel::setStep( int32_t newValue )
	{
		data.value = newValue;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::setRange( int32_t newRangeMax )
	{
		data.rangeMax = newRangeMax;

		if ( progress )
		{
			doUpdate();
		}
	}

	int32_t ProgressBar::ProgressLabel::incRange( int32_t mod )
	{
		data.rangeMax += mod;

		if ( progress )
		{
			doUpdate();
		}

		return getIndex();
	}

	int32_t ProgressBar::ProgressLabel::getIndex()const
	{
		return data.value;
	}

	void ProgressBar::ProgressLabel::set( castor::StringView newLabel
		, int32_t newRangeMax
		, int32_t newValue )
	{
		data.label = newLabel;
		data.rangeMax = newRangeMax;
		data.value = newValue;

		if ( progress )
		{
			doUpdate();
		}
	}

	void ProgressBar::ProgressLabel::doUpdate()
	{
		if ( updateEvent )
		{
			updateEvent->skip();
			updateEvent = nullptr;
		}

		CU_Require( data.value <= data.rangeMax );
		auto update = data;
		updateEvent = listener->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
			, [this, update]()
			{
				updateEvent = nullptr;

				if ( progress )
				{
					progress->setTitle( castor::string::toU32String( update.title ) );
					progress->setCaption( castor::string::toU32String( update.label ) );
					progress->setProgress( 0 );
					progress->setRange( castor::makeRange( 0, update.rangeMax ) );
					progress->setProgress( update.value );
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
		m_global.update( globalProgress );
		m_local.update( localProgress );
	}

	void ProgressBar::initGlobalRange( uint32_t value )
	{
		m_global.initRange( int32_t( value ) );
	}

	uint32_t ProgressBar::incGlobalRange( uint32_t value )
	{
		return uint32_t( m_global.incRange( int32_t( value ) ) );
	}

	void ProgressBar::stepGlobal( castor::String const & globalTitle )
	{
		m_global.setTitle( globalTitle );
		m_global.step();
	}

	void ProgressBar::setGlobalStep( castor::String const & title, uint32_t count )
	{
		m_global.setTitle( title );
		m_global.setStep( int32_t( count ) );
	}

	void ProgressBar::initLocalRange( castor::String const & globalLabel
		, uint32_t value )
	{
		m_global.setLabel( globalLabel );
		m_local.initRange( int32_t( value ) );
	}

	void ProgressBar::stepLocal( castor::String const & localLabel )
	{
		m_local.step( localLabel );
	}

	void ProgressBar::setLocalStep( castor::String const & label, uint32_t count )
	{
		m_local.setStep( label, int32_t( count ) );
	}

	void ProgressBar::setGlobalTitle( castor::String const & globalTitle )
	{
		m_global.setTitle( globalTitle );
	}

	void ProgressBar::stepGlobalStartLocal( castor::String const & globalLabel
			, uint32_t rangeMax )
	{
		m_global.setLabel( globalLabel );
		m_local.initRange( int32_t( rangeMax ) );
	}

	void ProgressBar::setLocal( castor::String const & globalLabel
			, castor::String const & localLabel
			, uint32_t rangeMax
			, uint32_t value )
	{
		m_global.setLabel( globalLabel );
		m_local.set( localLabel, int32_t( rangeMax ), int32_t( value ) );
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

	uint32_t incProgressBarGlobalRange( ProgressBar * progress
		, uint32_t increment )
	{
		if ( progress )
		{
			return progress->incGlobalRange( increment );
		}

		return 0u;
	}

	void setProgressBarGlobalTitle( ProgressBar * progress
		, castor::String const & globalTitle )
	{
		if ( progress )
		{
			progress->setGlobalTitle( globalTitle );
		}
	}

	void setProgressBarGlobalStep( ProgressBar * progress
		, castor::String const & globalTitle
		, uint32_t count )
	{
		if ( progress )
		{
			progress->setGlobalStep( globalTitle, count );
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

	void stepProgressBarGlobalStartLocal( ProgressBar * progress
		, castor::String const & globalLabel
		, uint32_t rangeMax )
	{
		if ( progress )
		{
			progress->stepGlobalStartLocal( globalLabel, rangeMax );
		}
	}

	void setProgressBarLocal( ProgressBar * progress
		, castor::String const & globalLabel
		, castor::String const & localLabel
		, uint32_t rangeMax
		, uint32_t value )
	{
		if ( progress )
		{
			progress->setLocal( globalLabel
				, localLabel
				, rangeMax
				, value );
		}
	}

	//*********************************************************************************************
}
