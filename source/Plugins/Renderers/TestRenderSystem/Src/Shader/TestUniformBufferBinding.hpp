/* See LICENSE file in root folder */
#ifndef ___C3DTRS_TestUniformBufferBinding_H___
#define ___C3DTRS_TestUniformBufferBinding_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/UniformBufferBinding.hpp>

namespace TestRender
{
	class TestUniformBufferBinding
		: public castor3d::UniformBufferBinding
	{
	public:
		TestUniformBufferBinding( castor3d::UniformBuffer & p_ubo, castor3d::ShaderProgram const & p_program );
		virtual ~TestUniformBufferBinding();

	private:
		void doBind( uint32_t p_index )const override;
	};
}

#endif
