/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CastorTest_BenchManager___
#define ___CastorTest_BenchManager___

#include "TestPrerequisites.hpp"

namespace Testing
{
	class BenchManager
	{
	public:
		BenchManager();
		~BenchManager();
		static void Register( BenchCaseUPtr p_bench );
		static void Register( TestCaseUPtr p_case );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static uint32_t ExecuteTests();

	private:
		static std::vector< BenchCaseUPtr > m_benchs;
		static std::vector< TestCaseUPtr > m_cases;
	};

	bool Register( BenchCaseUPtr p_bench );
	bool Register( TestCaseUPtr p_case );

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
