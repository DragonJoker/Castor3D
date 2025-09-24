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
			virtual bool startRecord( c3d::Size const & size, int wantedFPS ) = 0;
			virtual bool isRecording() = 0;
			virtual bool updateTime() = 0;
			virtual bool recordFrame( c3d::PxBufferBaseRPtr buffer ) = 0;
			virtual void stopRecord() = 0;
		};

	public:
		Recorder();

		inline bool startRecord( c3d::Size const & size, int wantedFPS )
		{
			bool result = !isRecording();
			if ( result )
				result = m_impl->startRecord( size, wantedFPS );
			return result;
		}

		inline bool isRecording()
		{
			return m_impl->isRecording();
		}

		inline bool updateTime()
		{
			return m_impl->updateTime();
		}

		inline bool recordFrame( c3d::PxBufferBaseRPtr buffer )
		{
			return m_impl->recordFrame( buffer );
		}

		inline void stopRecord()
		{
			if ( isRecording() )
				m_impl->stopRecord();
		}

	private:
		c3d::RawUniquePtr< IRecorderImpl > m_impl;
	};
}

#endif
