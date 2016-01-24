#include "Skeleton.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	Skeleton::Skeleton()
		: Animable()
		, m_globalInverse( 1.0_r )
	{
	}

	Skeleton::~Skeleton()
	{
	}

	void Skeleton::AddBone( BoneSPtr p_pBone )
	{
		m_bones.push_back( p_pBone );
	}
}

