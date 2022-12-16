/* See LICENSE file in root folder */
#ifndef ___CastorTest_BenchManager___
#define ___CastorTest_BenchManager___

#include "CastorTestPrerequisites.hpp"

namespace Testing
{
	class BenchManager
	{
	public:
		BenchManager();
		~BenchManager();
		static void registerType( BenchCaseUPtr bench );
		static void registerType( TestCaseUPtr test );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static uint32_t ExecuteTests();

	private:
		static std::vector< BenchCaseUPtr > m_benchs;
		static std::vector< TestCaseUPtr > m_cases;
	};

	bool registerType( BenchCaseUPtr bench );
	bool registerType( TestCaseUPtr test );

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
