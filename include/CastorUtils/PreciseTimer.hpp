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
#ifndef ___Castor_PreciseTimer___
#define ___Castor_PreciseTimer___

namespace Castor
{ namespace Utils
{
	//! The precise timer representation
	/*!
	Allows precise timing
	*/
	class PreciseTimer
	{
	private:
		static long long sm_frequency;	//!< The processor frequency
	private:
		long long m_previousTime;		//!< The previous retrieved time

	public:
		/**
		 * Constructor
		 */
		PreciseTimer();
		/**
		 * Destructor
		 */
		~PreciseTimer();

	public:
		/**
		 * Returns the time elapsed since the last call
		 */
		double Time();
		/**
		 * Returns the time elapsed between the last call and the given time
		 */
		double TimeDiff( long long p_time);

	public:
		/**
		 * Returns the last call time
		 */
		inline long long SaveTime()const { return m_previousTime; }
	};

	//! The repeated timer representation
	/*!
	Allows precise repeat timing
	*/
	class RepeatTimer
	{
	private:
		long long m_previousTime;		//!< The last call time
		double m_repeatTime;			//!< The wanted repeat time

	private:
		static long long sm_frequency;	//!< The performance frequency

	public:
		/**
		 * Constructor
		 */
		RepeatTimer( double p_repeatTime);
		/**
		 * Destructor
		 */
		~RepeatTimer();

	public:
		/**
		 * Returns true if this is called more than repeatTime after the last call
		 */
		bool Time();
		inline double GetRepeatTime()const { return m_repeatTime; };
	};
}
}

#endif
