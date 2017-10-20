/* See LICENSE file in root folder */
#ifndef ___GLT_GlTransformFeedbackTest_H___
#define ___GLT_GlTransformFeedbackTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <cstring>

namespace Testing
{
	class GlTransformFeedbackTest
		: public GlTestCase
	{
	public:
		explicit GlTransformFeedbackTest( castor3d::Engine & engine );
		virtual ~GlTransformFeedbackTest();

	private:
		void doRegisterTests()override;

	private:
		void Creation();
		void BasicCompute();
		void GeometryShader();
		void VariableFeedback();
		void ParticleSystem();
		void TwoWaysParticleSystem();
	};
}

#endif
