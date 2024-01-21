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
		C3D_API uint32_t incGlobalRange( uint32_t value );
		C3D_API void stepGlobal( castor::String const & globalTitle );
		C3D_API void setGlobalStep( castor::String const & globalTitle, uint32_t count );

		C3D_API void initLocalRange( castor::String const & globalLabel
			, uint32_t value );
		C3D_API void stepLocal( castor::String const & label );
		C3D_API void setLocalStep( castor::String const & label, uint32_t count );

		C3D_API void setGlobalTitle( castor::String const & globalTitle );
		C3D_API void stepGlobalStartLocal( castor::String const & globalLabel
			, uint32_t rangeMax );
		C3D_API void setLocal( castor::String const & globalLabel
			, castor::String const & localLabel
			, uint32_t rangeMax
			, uint32_t value );

		void lock()
		{
			m_mutex.lock();
		}

		void unlock()
		{
			m_mutex.unlock();
		}

	private:
		struct ProgressData
		{
			castor::String title{};
			castor::String label{};
			int32_t value{};
			int32_t rangeMax{};
		};

		struct ProgressLabel
		{
			void update( ProgressCtrlRPtr progress );
			void initRange( int32_t mod );
			void setTitle( castor::StringView value );
			void setLabel( castor::StringView value );
			void step( castor::StringView label );
			void step();
			void setStep( castor::StringView label, int32_t value );
			void setStep( int32_t value );
			void setRange( int32_t max );
			int32_t incRange( int32_t mod );
			int32_t getIndex()const;
			void set( castor::StringView label
				, int32_t rangeMax
				, int32_t value );

			FrameListenerRPtr listener;
			ProgressCtrlRPtr progress;
			ProgressData data;
			CpuFrameEvent * updateEvent{};

		private:
			void doUpdate();
		};

	private:
		FrameListenerRPtr m_listener;
		ProgressLabel m_global;
		ProgressLabel m_local;
		std::mutex m_mutex;
	};
}

#endif
