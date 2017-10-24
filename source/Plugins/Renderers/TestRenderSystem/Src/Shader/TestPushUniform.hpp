/* See LICENSE file in root folder */
#ifndef ___TRS_POINT_FRAME_VARIABLE_H___
#define ___TRS_POINT_FRAME_VARIABLE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/Uniform/PushUniform.hpp>

namespace TestRender
{
	template< castor3d::UniformType Type >
	class TestPushUniform
		: public castor3d::TPushUniform< Type >
	{
	public:
		explicit TestPushUniform( TestShaderProgram & p_program, uint32_t p_occurences );
		TestPushUniform();
		virtual ~TestPushUniform();

	private:
		virtual bool doInitialise();
		virtual void doUpdate()const;
	};
}

#include "TestPushUniform.inl"

#endif
