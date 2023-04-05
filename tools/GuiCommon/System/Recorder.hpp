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
			virtual ~IRecorderImpl() = default;
			virtual bool StartRecord( castor::Size const & size, int wantedFPS ) = 0;
			virtual bool IsRecording() = 0;
			virtual bool UpdateTime() = 0;
			virtual bool RecordFrame( castor::PxBufferBaseRPtr buffer ) = 0;
			virtual void StopRecord() = 0;
		};

	public:
		Recorder();

		inline bool StartRecord( castor::Size const & size, int wantedFPS )
		{
			bool result = !IsRecording();

			if ( result )
			{
				result = m_impl->StartRecord( size, wantedFPS );
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

		inline bool RecordFrame( castor::PxBufferBaseRPtr buffer )
		{
			return m_impl->RecordFrame( buffer );
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
