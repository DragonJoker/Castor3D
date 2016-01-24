#include "Skeleton.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	Skeleton::Skeleton()
		: m_globalInverse( 1 )
	{
	}

	Skeleton::~Skeleton()
	{
	}

	void Skeleton::AddBone( BoneSPtr p_bone )
	{
		m_bones.push_back( p_bone );
	}

	void Skeleton::AddAnimation( AnimationSPtr p_animation )
	{
		if ( m_animations.find( p_animation->GetName() ) == m_animations.end() )
		{
			m_animations.insert( std::make_pair( p_animation->GetName(), p_animation ) );
		}
	}
}
