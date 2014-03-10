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
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#	include <process.h>
#	pragma warning( pop )
#else
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <CastorUtils/String.hpp>
#include <CastorUtils/Logger.hpp>
#include <CastorUtils/PreciseTimer.hpp>

namespace CastorUtilsTest
{
#	define BENCH_CLASS_NAME( Name ) BenchCase_##Name
#	define TEST_CLASS_NAME( Name ) TestCase_##Name

	class BenchCase
	{
	private:
		std::string				m_strName;
		double					m_dCumulativeTimes;
		Castor::PreciseTimer	m_preciseTimer;
		uint64_t				m_uiTotalExecutions;

	public:
		BenchCase( std::string const & p_strName );
		virtual ~BenchCase();
		void Bench();
		inline std::string const & GetName()const { return m_strName; }
		inline double GetCumulativeTime()const { return m_dCumulativeTimes; }
		inline uint64_t GetTotalExecutions()const { return m_uiTotalExecutions; }

	protected:
		virtual void DoBench()=0;
	};

	class TestCase
	{
	private:
		std::string	m_strName;

	public:
		TestCase( std::string const & p_strName );
		virtual ~TestCase();
		virtual void Execute( uint32_t & p_errCount, uint32_t & p_testCount )=0;
		inline std::string const & GetName()const { return m_strName; }
	};
	
	DECLARE_SMART_PTR( BenchCase );
	DECLARE_SMART_PTR( TestCase );

	class BenchFactory
	{
	private:
		static std::vector< std::pair< uint64_t, BenchCaseSPtr > > m_arrayBenchs;
		static std::vector< TestCaseSPtr > m_arrayTests;

	public:
		BenchFactory();
		~BenchFactory();
		static void Register( uint64_t p_ui64Calls, BenchCaseSPtr p_pBench );
		static void Register( TestCaseSPtr p_pTest );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static void ExecuteTests();
	};
	
	bool RegisterBench( uint64_t p_ui64Calls, BenchCaseSPtr p_pBench );
	bool RegisterTest( TestCaseSPtr p_pCode );
	
#	define	BENCHMARK( Name, Calls )																									\
		class BENCH_CLASS_NAME( Name ) : public BenchCase																				\
		{																																\
		public:																															\
			static const bool m_bRegistered;																							\
			BENCH_CLASS_NAME( Name )()																									\
				:	BenchCase( #Name )																									\
			{																															\
			}																															\
		private:																														\
			virtual void DoBench();																										\
		};																																\
		const bool BENCH_CLASS_NAME( Name )::m_bRegistered = RegisterBench( Calls, std::make_shared< BENCH_CLASS_NAME( Name ) >() );	\
		void BENCH_CLASS_NAME( Name )::DoBench()
	
#	define	UNITTEST( Name )																								\
		class TEST_CLASS_NAME( Name ) : public TestCase																		\
		{																													\
		public:																												\
			static const bool m_bRegistered;																				\
			TEST_CLASS_NAME( Name )()																						\
				:	TestCase( #Name )																						\
			{																												\
			}																												\
			virtual void Execute( uint32_t & p_errCount, uint32_t & p_testCount );											\
		};																													\
		const bool TEST_CLASS_NAME( Name )::m_bRegistered = RegisterTest( std::make_shared< TEST_CLASS_NAME( Name ) >() );	\
		void TEST_CLASS_NAME( Name )::Execute(  uint32_t & p_errCount, uint32_t & p_testCount )

	
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

#define BENCHLOOP( p_iMax )					\
		BenchFactory::ExecuteTests();		\
		for( int i = 0; i < p_iMax; ++i )	\
		{									\
			BenchFactory::ExecuteBenchs();	\
		}									\
		if( p_iMax > 1 )					\
		{									\
			BenchFactory::BenchsSummary();	\
		}
	
#define TEST_CHECK( x ) p_testCount++;if( !(x) ) { Logger::LogWarning( "Test "#x" failed" );p_errCount++; }
#define TEST_REQUIRE( x ) p_testCount++;if( !(x) ) { throw std::exception( #x ); }
}

#endif