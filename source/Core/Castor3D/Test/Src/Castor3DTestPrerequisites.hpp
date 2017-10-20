/* See LICENSE file in root folder */
#ifndef ___C3DT_TestPrerequisites___
#define ___C3DT_TestPrerequisites___

#include <UnitTest.hpp>
#include <Benchmark.hpp>

#include <Render/TestRenderSystem.hpp>
#include <Miscellaneous/WindowHandle.hpp>

#include "Castor3DTestCommon.hpp"

namespace Testing
{
	class TestWindowHandle
		: public castor3d::IWindowHandle
	{
	public:
		TestWindowHandle()
		{
		}

		virtual ~TestWindowHandle()
		{
		}

		virtual operator bool()
		{
			return true;
		}
	};
}

#endif
