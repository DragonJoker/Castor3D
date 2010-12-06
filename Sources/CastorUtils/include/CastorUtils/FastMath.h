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
#ifndef ___Castor_MinMax___
#define ___Castor_MinMax___

//In case someone tries to create macros for these -_- template versions are better...
#ifdef min
#	undef min
#endif

#ifdef max
#	undef max
#endif

#ifdef abs
#	undef abs
#endif

#ifdef nabs
#	undef nabs
#endif

namespace Castor
{ namespace Math
{
	//! Integer factorial
	template <size_t N> struct facti
	{
		enum { Value = N * facti<N - 1>::Value };
	};
	//! Integer factorial end case
	template <> struct facti<0>
	{
		enum { Value = 1 };
	};

	// Floating factorial
	template <int N> inline real factr()
	{
		return N * factr<N - 1>();
	}
	//! Floating factorial
	template <> inline real factr<0>()
	{
		return real( 1);
	}

	//! Power
	template <int N> inline real power( real x)
	{
		return x * power<N - 1>( x);
	}
	//! Power end cases
	template <> inline real power<1>( real x)
	{
		return x;
	}
	template <> inline real power<0>( real x)
	{
		return real( 1);
	}

	//! Exponential, x > 0
	template <int N> inline real Exp_( real x)
	{
		return Exp_<N - 1>( x) + power<N>( x) / factr<N>();
	}
	//! Exponential end case
	template <> inline real Exp_<0>( real x)
	{
		return 0.0;
	}
	//! Exponential
	template <int N> inline real exp( real x)
	{
		return x < 0 ? 1 / Exp_<N>( -x) : Exp_<N>( x);
	}

	template <int N> inline real ln( real x)
	{
		return ln<N - 1>( x) + (N % 2 == 0 ? -1 : 1) * power<N>( x) / N;
	}
	template <> inline real ln<1>( real x)
	{
		return real( 0);
	}

	template <typename T>
	inline const T & minmax( const T & p_min, const T & p_value, const T & p_max)
	{
		if (p_value < p_min)
		{
			return p_min;
		}

		if (p_value > p_max)
		{
			return p_max;
		}

		return p_value;
	}

	template <typename T>
	inline void clamp( const T & p_min, T & p_value, const T & p_max)
	{
		if (p_value < p_min)
		{
			p_value = p_min;
			return;
		}

		if (p_value > p_max)
		{
			p_value = p_max;
		}
	}

	template <typename T>
	inline T clamp( const T & p_min, const T & p_value, const T & p_max)
	{
		T l_tReturn = p_value;

		if (p_value < p_min)
		{
			l_tReturn = p_min;
		}
		else if (p_value > p_max)
		{
			l_tReturn = p_max;
		}

		return l_tReturn;
	}
}
}

#endif
