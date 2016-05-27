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

#include "Castor3DTestUtils.hpp"

namespace Castor3D
{
	namespace Testing
	{
		static Castor::String RENDERER_TYPE_TEST = cuT( "test" );

		class TestCase
			: public ::Testing::TestCase
		{
		public:
			inline TestCase( std::string const & p_name, Engine & p_engine )
				: ::Testing::TestCase{ p_name }
				, m_engine{ p_engine }
			{
			}

		protected:
			Engine & m_engine;
		};
	}
}

#endif
