#include "Skeleton.hpp"

#include "Animation.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	Skeleton::Skeleton()
		: m_globalInverse( 1 )
	{
	}

	Skeleton::~Skeleton()
	{
		Animable::CleanupAnimations();
	}

	void Skeleton::AddBone( BoneSPtr p_bone )
	{
		m_bones.push_back( p_bone );
	}

	void Skeleton::SetBoneParent( BoneSPtr p_bone, BoneSPtr p_parent )
	{
		if ( std::find( m_bones.begin(), m_bones.end(), p_bone ) == m_bones.end() )
		{
			CASTOR_EXCEPTION( "Skeleton::SetBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		if ( std::find( m_bones.begin(), m_bones.end(), p_parent ) == m_bones.end() )
		{
			CASTOR_EXCEPTION( "Skeleton::SetBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		p_parent->AddChild( p_bone );
		p_bone->SetParent( p_parent );
	}
}
