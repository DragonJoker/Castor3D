/*
See LICENSE file in root folder
*/
#ifndef ___C3DGL_GlUniformBufferBinding_H___
#define ___C3DGL_GlUniformBufferBinding_H___

#include <Shader/UniformBufferBinding.hpp>

#include "GlRenderSystemPrerequisites.hpp"
#include "Common/GlHolder.hpp"

namespace GlRender
{
	class GlUniformBufferBinding
		: public castor3d::UniformBufferBinding
		, public Holder
	{
	public:
		GlUniformBufferBinding( OpenGl & p_gl
			, castor3d::UniformBuffer & p_ubo
			, GlShaderProgram const & p_program );
		virtual ~GlUniformBufferBinding();

	private:
		void doBind( uint32_t p_index )const override;

	private:
		int m_blockIndex;
	};
}

#endif
