/* See LICENSE file in root folder */
#ifndef ___GLT_GlComputeShaderTest_H___
#define ___GLT_GlComputeShaderTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class GlComputeShaderTest
		: public GlTestCase
	{
	public:
		explicit GlComputeShaderTest( castor3d::Engine & engine );
		virtual ~GlComputeShaderTest();

	private:
		void doRegisterTests()override;

	private:
		void SimpleCompute();
		void AtomicCounter();
		void TwoStorages();
		void TwoStoragesAndAtomicCounter();
	};
}

#endif
