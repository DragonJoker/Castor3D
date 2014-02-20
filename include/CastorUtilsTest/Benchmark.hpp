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
#ifndef ___CUT_CastorUtilsTest___
#define ___CUT_CastorUtilsTest___

#if defined( _WIN32 )
#	include <process.h>
#else
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <CastorUtils/String.hpp>
#include <CastorUtils/Logger.hpp>
#include <CastorUtils/PreciseTimer.hpp>

namespace CastorUtilsTest
{
#	define BENCHMARK_CLASS_NAME( Name ) Benchmark_##Name
#	define CODE_CLASS_NAME( Name ) Code_##Name

	class BenchmarkBase
	{
	private:
		std::string				m_strName;
		double					m_dCumulativeTimes;
		Castor::PreciseTimer	m_preciseTimer;
		uint64_t				m_uiTotalExecutions;

	public:
		BenchmarkBase( std::string const & p_strName );
		virtual ~BenchmarkBase();
		void Bench();
		inline std::string const & GetName()const { return m_strName; }
		inline double GetCumulativeTime()const { return m_dCumulativeTimes; }
		inline uint64_t GetTotalExecutions()const { return m_uiTotalExecutions; }

	protected:
		virtual void DoBench()=0;
	};

	class CodeBase
	{
	private:
		std::string	m_strName;

	public:
		CodeBase( std::string const & p_strName );
		virtual ~CodeBase();
		virtual void Execute()=0;
		inline std::string const & GetName()const { return m_strName; }
	};
	
	DECLARE_SMART_PTR( BenchmarkBase );
	DECLARE_SMART_PTR( CodeBase );

	class BenchFactory
	{
	private:
		static std::vector< std::pair< uint64_t, BenchmarkBaseSPtr > > m_arrayBenchs;
		static std::vector< CodeBaseSPtr > m_arrayCodes;

	public:
		BenchFactory();
		~BenchFactory();
		static void Register( uint64_t p_ui64Calls, BenchmarkBaseSPtr p_pBench );
		static void Register( CodeBaseSPtr p_pCode );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static void ExecuteCodes();
	};
	
	bool RegisterBenchmark( uint64_t p_ui64Calls, BenchmarkBaseSPtr p_pBenchmark );
	bool RegisterCode( CodeBaseSPtr p_pCode );
	
#	define	BENCHMARK( Name, Calls )																												\
		class BENCHMARK_CLASS_NAME( Name ) : public BenchmarkBase																					\
		{																																			\
		public:																																		\
			static const bool m_bRegistered;																										\
			BENCHMARK_CLASS_NAME( Name )()																											\
				:	BenchmarkBase( #Name )																											\
			{																																		\
			}																																		\
		private:																																	\
			virtual void DoBench();																													\
		};																																			\
		const bool BENCHMARK_CLASS_NAME( Name )::m_bRegistered = RegisterBenchmark( Calls, std::make_shared< BENCHMARK_CLASS_NAME( Name ) >() );	\
		void BENCHMARK_CLASS_NAME( Name )::DoBench()
	
#	define	CODE( Name )																									\
		class CODE_CLASS_NAME( Name ) : public CodeBase																		\
		{																													\
		public:																												\
			static const bool m_bRegistered;																				\
			CODE_CLASS_NAME( Name )()																						\
				:	CodeBase( #Name )																						\
			{																												\
			}																												\
			virtual void Execute();																							\
		};																													\
		const bool CODE_CLASS_NAME( Name )::m_bRegistered = RegisterCode( std::make_shared< CODE_CLASS_NAME( Name ) >() );	\
		void CODE_CLASS_NAME( Name )::Execute()

	
	///
	/// \func DoNotOptimizeAway
	///
	/// From Andrei Alexandrescu
	///
	template< class T > void DoNotOptimizeAway( T && datum )
	{
#if defined( _WIN32 )
		if( _getpid() == 1 )
#else
		if( getpid() == 1 )
#endif
		{
			const void * p = &datum;
			putchar( *static_cast< const char* >( p ) );
		}
	}

#	define BENCHLOOP( p_iMax )				\
		for( int i = 0; i < p_iMax; ++i )	\
		{									\
			BenchFactory::ExecuteBenchs();	\
		}									\
		if( p_iMax > 1 )					\
		{									\
			BenchFactory::BenchsSummary();	\
		}									\
		BenchFactory::ExecuteCodes()
}

#endif