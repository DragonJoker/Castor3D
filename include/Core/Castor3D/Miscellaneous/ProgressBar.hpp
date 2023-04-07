/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressBar_H___
#define ___C3D_ProgressBar_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class ProgressBar
	{
	public:
		C3D_API ProgressBar( Engine & engine
			, ProgressCtrlRPtr progress );

		C3D_API void update( ProgressCtrlRPtr progress );
		C3D_API void setTitle( castor::String const & value );
		C3D_API void setLabel( castor::String const & value );
		C3D_API void step( castor::String const & label );
		C3D_API void step();
		C3D_API void setRange( int32_t max );
		C3D_API void incRange( int32_t mod );
		C3D_API int32_t getIndex()const;

		void lock()
		{
			m_mutex.lock();
		}

		void unlock()
		{
			m_mutex.unlock();
		}

	private:
		void doSetTitle( castor::U32String const & value );
		void doSetLabel( castor::U32String const & value );
		void doStep();

	private:
		FrameListenerRPtr m_listener;
		castor::String m_title;
		castor::String m_label;
		ProgressCtrlRPtr m_progress;
		CpuFrameEvent * m_titleEvent{};
		CpuFrameEvent * m_labelEvent{};
		CpuFrameEvent * m_rangeEvent{};
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
		, int32_t value );
}

#endif
