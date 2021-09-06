/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressBar_H___
#define ___C3D_ProgressBar_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class ProgressBar
	{
	public:
		C3D_API ProgressBar( Engine & engine
			, OverlayResPtr parent
			, OverlayResPtr bar
			, TextOverlaySPtr title
			, TextOverlaySPtr label
			, uint32_t max );

		C3D_API void setTitle( castor::String const & value );
		C3D_API void setLabel( castor::String const & value );
		C3D_API void step( castor::String const & label );
		C3D_API void step();

		void setRange( uint32_t max )
		{
			m_index.updateRange( castor::makeRange( 0u, max ) );
		}

		void incRange( uint32_t mod )
		{
			setRange( m_index.range().getMax() + mod );
		}

		uint32_t getIndex()const
		{
			return m_index.value();
		}

		void lock()
		{
			m_mutex.lock();
		}

		void unlock()
		{
			m_mutex.unlock();
		}

	private:
		Engine & m_engine;
		FrameListenerRes m_listener;
		castor::RangedValue< uint32_t > m_index;
		castor::String m_title;
		OverlayRPtr m_progress;
		OverlayRPtr m_progressBar;
		TextOverlaySPtr m_progressTitle;
		TextOverlaySPtr m_progressLabel;
		std::mutex m_mutex;
	};

	C3D_API void setProgressBarTitle( ProgressBar * progress
		, castor::String const & value );
	C3D_API void setProgressBarLabel( ProgressBar * progress
		, castor::String const & value );
	C3D_API void stepProgressBar( ProgressBar * progress
		, castor::String const & label );
	C3D_API void stepProgressBar( ProgressBar * progress );
	C3D_API void incProgressBarRange( ProgressBar * progress
		, uint32_t value );
}

#endif
