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
#ifndef ___Castor_MinMax___
#define ___Castor_MinMax___

#include "Value.hpp"

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
	//************************************************************************************************
	//! Factorial
	template <typename T, int N> struct fact
	{
		inline T operator ()()
		{
			return N * fact<T, N - 1>()();
		}
	};
	//! Factorial end case
	template <typename T> struct fact<T, 0>
	{
		inline T operator ()()
		{
			return T( 1);
		}
	};
	//************************************************************************************************
	//! Power
	template <typename T, int N> struct power
	{
		inline T operator ()( T x)
		{
			return power<T, N / 2>()( x) * power<T, N / 2>()( x) * power<T, N % 2>()( x);
		}
	};
	//! Power end cases
	template <typename T> struct power<T, 1>
	{
		inline T operator ()( T x)
		{
			return x;
		}
	};
	template <typename T> struct power<T, 0>
	{
		inline T operator ()( T x)
		{
			return T( 1);
		}
	};
	//************************************************************************************************
	//! Exponential
	template <int N, typename T> struct expPos
	{
		inline double operator()( T x)
		{
			return expPos<N - 1, T>()(x) + power<T, N>()(x) / fact<double, N>()();
		}
	};
	template <typename T> struct expPos<0, T>
	{
		inline double operator()( T x)
		{
			return 1.0;
		}
	};
	template <typename T> struct exp
	{
	private:
		enum
		{
			Precision = 40
		};
	public:
		inline double operator()( T x)
		{
			return (x < 0.0 ? 1.0 / expPos<Precision, T>()( -x) : expPos<Precision, T>()( x));
//			return power<double, Precision>()( 1.0 + double( x) / double( Precision));
		}
	};
	//************************************************************************************************
	//! Neperian Logarithm
	template <int N, typename T> struct lnN
	{
		inline double operator()( T x)
		{
			return lnN<N - 1, T>()( x) + power<double, 2 * N + 1>()( (double( x) - 1) / (double( x) + 1)) / (2 * N + 1);
		}
	};
	//! Neperian Logarithm end case
	template <typename T> struct lnN<0, T>
	{
		inline double operator()( T x)
		{
			return (double( x) - 1) / (double( x) + 1);
		}
	};
	template <typename T> struct ln
	{
	private:
		enum
		{
			Precision = 20
		};
	public:
		inline double operator()( T x)
		{
			return 2.0 * lnN<Precision, T>()( x);
		}
	};
	//************************************************************************************************
	//! Returns value if it is in the range [min,max], min if lower than min, max if larger than max
	template <typename T>
	inline T const & minmax( T const & p_min, T const & p_value, T const & p_max)
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
	//! Puts the value in the range [min, max]
	template <typename T>
	inline void clamp( T const &, T & p_value, T const & p_max)
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
	//! Returns value if it is in the range [min,max], min if lower than min, max if larger than max
	template <typename T>
	inline T clamp( T const & p_min, T const & p_value, T const & p_max)
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
	inline bool is_nan( double x)
	{
		volatile double temp = x;
		return temp != x;
	}

	inline int is_inf( double x)
	{
		volatile double temp = x;

		if ((temp == x) && ((temp - x) != 0.0))
		{
			return (x < 0.0 ? -1 : 1);
		}
		else
		{
			return 0;
		}
	}
	template <typename T>
	inline T abs( T const & a)
	{
		return a < 0 ? -a : a;
	}
	template <typename T>
	inline T min( T a, T b)
	{
		return a < b ? a : b;
	}
	template <typename T>
	inline T max( T a, T b)
	{
		return a > b ? a : b;
	}
}
}

#endif
