#include "SkinningSsbo.hpp"

#include "Engine.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	SkinningSsbo::SkinningSsbo( Engine & p_engine )
		: m_ssbo{ p_engine }
	{
	}

	SkinningSsbo::~SkinningSsbo()
	{
	}

	void SkinningSsbo::Update( AnimatedSkeleton const & p_skeleton
		, uint32_t p_index )const
	{
		//p_skeleton.FillShader( m_bonesMatrix );
		//m_ssbo.Update();
	}
}
