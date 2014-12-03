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
#ifndef ___CUT_Benchmark___
#define ___CUT_Benchmark___

#include "TestPrerequisites.hpp"

namespace Testing
{
	///
	/// \func DoNotOptimizeAway
	///
	/// From Andrei Alexandrescu
	///
	template< class T >
	static inline void DoNotOptimizeAway( T && datum )
	{
#if defined( _WIN32 )

		if ( _getpid() == 1 )
#else
		if ( getpid() == 1 )
#endif
		{
			const void * p = &datum;
			putchar( *static_cast< const char * >( p ) );
		}
	}

	class BenchCase
	{
	private:
		std::string				m_strName;
		double					m_dCumulativeTimes;
		Castor::PreciseTimer	m_preciseTimer;
		uint64_t				m_uiTotalExecutions;
		std::string				m_strSummary;
		typedef std::function< void() > CallbackBench;

	public:
		BenchCase( std::string const & p_strName );
		virtual ~BenchCase();
		virtual void Execute() = 0;
		inline std::string const & GetSummary()const
		{
			return m_strSummary;
		}

	protected:
		void DoBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls );
	};

#	define BENCHMARK( Name, Calls ) DoBench( #Name, [&](){ Name(); }, Calls )
}

#endif
