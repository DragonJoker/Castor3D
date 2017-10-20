/* See LICENSE file in root folder */
#ifndef ___GLT_GlPassBufferTest_H___
#define ___GLT_GlPassBufferTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class GlPassBufferTest
		: public GlTestCase
	{
	public:
		explicit GlPassBufferTest( castor3d::Engine & engine );
		virtual ~GlPassBufferTest();

	private:
		void doRegisterTests()override;

	private:
		void Legacy();
		void MetallicRoughness();
		void SpecularGlossiness();
	};
}

#endif
