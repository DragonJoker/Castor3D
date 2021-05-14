/*
See LICENSE file in root folder
*/
#ifndef ___GC_RECORDER_H___
#define ___GC_RECORDER_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

namespace GuiCommon
{
	class Recorder
	{
	public:
		class IRecorderImpl
		{
		public:
			IRecorderImpl() {}
			virtual ~IRecorderImpl() {}
			virtual bool StartRecord( castor::Size const & p_size, int p_wantedFPS ) = 0;
			virtual bool IsRecording() = 0;
			virtual bool UpdateTime() = 0;
			virtual bool RecordFrame( castor::PxBufferBaseSPtr p_buffer ) = 0;
			virtual void StopRecord() = 0;
		};

	public:
		Recorder();
		~Recorder();

		inline bool StartRecord( castor::Size const & p_size, int p_wantedFPS )
		{
			bool result = !IsRecording();

			if ( result )
			{
				result = m_impl->StartRecord( p_size, p_wantedFPS );
			}

			return result;
		}

		inline bool IsRecording()
		{
			return m_impl->IsRecording();
		}

		inline bool UpdateTime()
		{
			return m_impl->UpdateTime();
		}

		inline bool RecordFrame( castor::PxBufferBaseSPtr p_buffer )
		{
			return m_impl->RecordFrame( p_buffer );
		}

		inline void StopRecord()
		{
			if ( IsRecording() )
			{
				m_impl->StopRecord();
			}
		}

	private:
		std::unique_ptr< IRecorderImpl > m_impl;
	};
}

#endif
