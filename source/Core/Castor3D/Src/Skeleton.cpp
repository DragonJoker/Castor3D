#include "Skeleton.hpp"

#include "Animation.hpp"
#include "Mesh.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

Skeleton::Skeleton( MeshSPtr p_mesh )
	: Animable( *p_mesh->GetOwner() )
	, m_mesh( p_mesh )
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

//*************************************************************************************************
