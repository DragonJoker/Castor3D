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
#ifndef ___C3DT_TestPrerequisites___
#define ___C3DT_TestPrerequisites___

#include <UnitTest.hpp>
#include <Benchmark.hpp>

#include <Render/TestRenderSystem.hpp>

#include "Castor3DTestUtils.hpp"

namespace Testing
{
#if !defined( NDEBUG )

	static Castor::Path const TEST_DATA_FOLDER{ DATA_FOLDER_DEBUG };

#else

	static Castor::Path const TEST_DATA_FOLDER{ DATA_FOLDER_RELEASE };

#endif

	class C3DTestCase
		: public TestCase
	{
	public:
		inline C3DTestCase( std::string const & p_name, Castor3D::Engine & p_engine )
			: TestCase{ p_name }
			, m_engine{ p_engine }
		{
		}

	protected:
		Castor3D::Engine & m_engine;
	};
}

#endif
