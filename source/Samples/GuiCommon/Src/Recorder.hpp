/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GC_RECORDER_H___
#define ___GC_RECORDER_H___

#include "GuiCommonPrerequisites.hpp"

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
			virtual bool StartRecord( Castor::Size const & p_size, int p_wantedFPS ) = 0;
			virtual bool IsRecording() = 0;
			virtual bool UpdateTime() = 0;
			virtual bool RecordFrame( Castor::PxBufferBaseSPtr p_buffer ) = 0;
			virtual void StopRecord() = 0;
		};

	public:
		Recorder();
		~Recorder();

		inline bool StartRecord( Castor::Size const & p_size, int p_wantedFPS )
		{
			bool l_result = !IsRecording();

			if ( l_result )
			{
				l_result = m_impl->StartRecord( p_size, p_wantedFPS );
			}

			return l_result;
		}

		inline bool IsRecording()
		{
			return m_impl->IsRecording();
		}

		inline bool UpdateTime()
		{
			return m_impl->UpdateTime();
		}

		inline bool RecordFrame( Castor::PxBufferBaseSPtr p_buffer )
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
