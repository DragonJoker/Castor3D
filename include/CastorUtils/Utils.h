/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_Utils___
#define ___Castor_Utils___

#ifdef _WIN32
	//In case windows.h was already included ...
#	ifndef _WINDOWS_

		extern "C"
		{
			__declspec( dllimport) void			__stdcall Sleep						( unsigned long);
			__declspec( dllimport) unsigned int	__stdcall GetWindowsDirectoryA		( char * lpBuffer, unsigned int uSize);
			__declspec( dllimport) void			__stdcall QueryPerformanceCounter	( long long * p_perfCount);
			__declspec( dllimport) void			__stdcall QueryPerformanceFrequency	( long long * p_perfFreq);
			__declspec( dllimport) int			__stdcall GetSystemMetrics			( int p_code);
		}
#	endif

#	define CASTOR_SLEEP( time) Sleep( time)

#elif defined( __GNUG__)

#	define CASTOR_SLEEP( time) sleep( time)
#	include <sys/time.h>
#	include <errno.h>
#	include <iostream>

namespace Castor
{ namespace Utils
{
	inline void Sleep( double p_time)
	{
		timespec l_timeToWait;
		timespec l_timeRemaining;
	
		l_timeToWait.tv_sec = static_cast <long>( p_time);
		l_timeToWait.tv_nsec = static_cast <long>( (p_time - l_timeToWait.tv_sec) * 1000000000);

		while (true)
		{
			if (nanosleep( & l_timeToWait, & l_timeRemaining) == 0)
			{
				return;
			}

			if (errno != EINTR)
			{	/*	2 types d'erreur possibles :
					- EFAULT (les paramètres ne sont pas dans l'espace d'adressage du programme),
					- EINVAL (le champ tv_nsec n'est pas dans l'intervalle [0, 999 999 999]).
				*/
				std::cerr << "Trouble encountered when sleeping : [" << strerror( errno) << "]" << std::endl;
				return;
			}

			l_timeToWait.tv_sec = l_timeRemaining.tv_sec;
			l_timeToWait.tv_nsec = l_timeRemaining.tv_nsec;
		}
	}
}
}

using Castor::Utils::Sleep;

#endif
#endif
