/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CUT_CastorUtilsObjectsPoolTest___
#define ___CUT_CastorUtilsObjectsPoolTest___

#include "UnitTest.hpp"
#include "Benchmark.hpp"

namespace Testing
{
	class CastorUtilsObjectsPoolTest
		: public TestCase
	{
	public:
		CastorUtilsObjectsPoolTest();
		virtual ~CastorUtilsObjectsPoolTest();
		virtual void Execute( uint32_t & p_errCount, uint32_t & p_testCount );

	private:
		void ObjectPoolTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void AlignedObjectPoolTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void FixedSizePerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void AllDeallPerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void ScatteredMemoryPerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void VariableSizePerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount );
		void UniquePoolTest( uint32_t & p_errCount, uint32_t & p_testCount );
	};
}

#endif
