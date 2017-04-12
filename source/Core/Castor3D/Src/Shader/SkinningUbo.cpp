#include "SkinningUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	SkinningUbo::SkinningUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferSkinning, *p_engine.GetRenderSystem() }
		, m_bonesMatrix{ *m_ubo.CreateUniform< UniformType::eMat4x4f >( ShaderProgram::Bones ) }
	{
	}

	SkinningUbo::~SkinningUbo()
	{
	}

	void SkinningUbo::Update( AnimatedSkeleton const & p_skeleton )const
	{
		p_skeleton.FillShader( m_bonesMatrix );
		m_ubo.Update();
	}
}
