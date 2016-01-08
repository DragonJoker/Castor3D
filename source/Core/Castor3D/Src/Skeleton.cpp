#include "Skeleton.hpp"

#include "Animation.hpp"
#include "Mesh.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

Skeleton::Skeleton( MeshSPtr p_mesh )
	: Animable( *p_mesh->GetOwner() )
	, m_wpMesh( p_mesh )
{
	m_mtxGlobalInverse.set_identity();
}

Skeleton::~Skeleton()
{
}

void Skeleton::AddBone( BoneSPtr p_pBone )
{
	m_arrayBones.push_back( p_pBone );
}

//*************************************************************************************************
