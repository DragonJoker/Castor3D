/* See LICENSE file in root folder */
#ifndef ___CastorTest_BenchManager___
#define ___CastorTest_BenchManager___

#include "CastorTestPrerequisites.hpp"

namespace Testing
{
	class BenchManager
	{
	public:
		static void registerType( BenchCasePtr bench );
		static void registerType( TestCasePtr test );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static uint32_t ExecuteTests();

	private:
		static std::vector< BenchCasePtr > m_benchs;
		static std::vector< TestCasePtr > m_cases;
	};

	bool registerType( BenchCasePtr bench );
	bool registerType( TestCasePtr test );

#define BENCHLOOP( iMax, ret )\
		ret = ::Testing::BenchManager::ExecuteTests();\
		for( uint32_t i = 0u; i < iMax; ++i )\
		{\
			::Testing::BenchManager::ExecuteBenchs();\
		}\
		if( iMax > 1 )\
		{\
			::Testing::BenchManager::BenchsSummary();\
		}
}

#endif
