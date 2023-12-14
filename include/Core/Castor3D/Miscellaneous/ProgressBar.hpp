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
			, ProgressCtrlRPtr globalProgress
			, ProgressCtrlRPtr localProgress = nullptr );

		C3D_API void update( ProgressCtrlRPtr globalProgress
			, ProgressCtrlRPtr localProgress = nullptr );

		C3D_API void initGlobalRange( uint32_t value );
		C3D_API void stepGlobal( castor::String const & globalTitle );

		C3D_API void initLocalRange( castor::String const & globalLabel
			, uint32_t value );
		C3D_API void stepLocal( castor::String const & label );

		void lock()
		{
			m_mutex.lock();
		}

		void unlock()
		{
			m_mutex.unlock();
		}

	private:
		struct ProgressLabel
		{
			void update( ProgressCtrlRPtr progress );
			void initRange( int32_t mod );
			void setTitle( castor::String const & value );
			void setLabel( castor::String const & value );
			void step( castor::String const & label );
			void step();
			void setRange( int32_t max );
			void incRange( int32_t mod );
			int32_t getIndex()const;

			FrameListenerRPtr listener;
			ProgressCtrlRPtr progress;
			castor::String title;
			castor::String label;
			CpuFrameEvent * titleEvent{};
			CpuFrameEvent * labelEvent{};
			CpuFrameEvent * rangeEvent{};

		private:
			void doSetTitle( castor::U32String const & value );
			void doSetLabel( castor::U32String const & value );
			void doStep();
		};

	private:
		FrameListenerRPtr m_listener;
		ProgressLabel m_global;
		ProgressLabel m_local;
		std::mutex m_mutex;
	};
}

#endif
