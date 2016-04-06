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
#ifndef ___CUT_BenchManager___
#define ___CUT_BenchManager___

#include "TestPrerequisites.hpp"

namespace Testing
{
	class BenchManager
	{
	public:
		BenchManager();
		~BenchManager();
		static void Register( BenchCaseSPtr p_pBench );
		static void Register( TestCaseSPtr p_pTest );
		static void ExecuteBenchs();
		static void BenchsSummary();
		static uint32_t ExecuteTests();

	private:
		static std::vector< BenchCaseSPtr > m_arrayBenchs;
		static std::vector< TestCaseSPtr > m_arrayTests;
	};

	bool Register( BenchCaseSPtr p_pBench );
	bool Register( TestCaseSPtr p_pCode );

#define BENCHLOOP( p_iMax, p_return )\
		p_return = Testing::BenchManager::ExecuteTests();\
		for( int i = 0; i < p_iMax; ++i )\
		{\
			Testing::BenchManager::ExecuteBenchs();\
		}\
		if( p_iMax > 1 )\
		{\
			Testing::BenchManager::BenchsSummary();\
		}
}

#endif
