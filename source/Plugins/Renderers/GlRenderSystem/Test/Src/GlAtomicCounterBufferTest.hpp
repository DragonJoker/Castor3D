/* See LICENSE file in root folder */
#ifndef ___GLT_GlAtomicCounterBufferTest_H___
#define ___GLT_GlAtomicCounterBufferTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class GlAtomicCounterBufferTest
		: public GlTestCase
	{
	public:
		explicit GlAtomicCounterBufferTest( castor3d::Engine & engine );
		virtual ~GlAtomicCounterBufferTest();

	private:
		void doRegisterTests()override;

	private:
		void Creation();
		void InOut();
	};
}

#endif
