#include "Skeleton.hpp"
#include "Animation.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

Skeleton::Skeleton( MeshSPtr p_pMesh )
	:	m_wpMesh( p_pMesh )
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
