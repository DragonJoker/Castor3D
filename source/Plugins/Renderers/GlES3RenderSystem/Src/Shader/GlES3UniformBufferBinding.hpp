/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GlES3UniformBufferBinding_H___
#define ___C3DGLES3_GlES3UniformBufferBinding_H___

#include <Shader/UniformBufferBinding.hpp>

#include "GlES3RenderSystemPrerequisites.hpp"
#include "Common/GlES3Holder.hpp"

namespace GlES3Render
{
	class GlES3UniformBufferBinding
		: public Castor3D::UniformBufferBinding
		, public Holder
	{
	public:
		GlES3UniformBufferBinding( OpenGlES3 & p_gl
			, Castor3D::UniformBuffer & p_ubo
			, GlES3ShaderProgram const & p_program );
		virtual ~GlES3UniformBufferBinding();

	private:
		void DoBind( uint32_t p_index )const override;

	private:
		int m_blockIndex;
	};
}

#endif
