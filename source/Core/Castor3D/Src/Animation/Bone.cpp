#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	Bone::Bone( Skeleton & p_skeleton )
		: m_skeleton( p_skeleton )
	{
		m_finalTransformation.set_identity();
	}

	Bone::~Bone()
	{
	}

	void Bone::AddChild( BoneSPtr p_bone )
	{
		if ( m_children.end() == std::find( m_children.begin(), m_children.end(), p_bone ) )
		{
			m_children.push_back( p_bone );
		}
	}
}
