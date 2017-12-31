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
		static void registerType( BenchCaseUPtr p_bench );
		static void registerType( TestCaseUPtr p_case );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static uint32_t ExecuteTests();

	private:
		static std::vector< BenchCaseUPtr > m_benchs;
		static std::vector< TestCaseUPtr > m_cases;
	};

	bool registerType( BenchCaseUPtr p_bench );
	bool registerType( TestCaseUPtr p_case );

#define BENCHLOOP( p_iMax, p_return )\
		p_return = ::Testing::BenchManager::ExecuteTests();\
		for( int i = 0; i < p_iMax; ++i )\
		{\
			::Testing::BenchManager::ExecuteBenchs();\
		}\
		if( p_iMax > 1 )\
		{\
			::Testing::BenchManager::BenchsSummary();\
		}
}

#endif
