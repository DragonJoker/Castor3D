#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Skeleton.hpp"
#include "Castor3D/Animation.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

Bone :: Bone( Skeleton & p_skeleton )
	:	m_skeleton	( p_skeleton	)
{
}

Bone :: ~Bone()
{
}

//*************************************************************************************************

Skeleton :: Skeleton( MeshSPtr p_pMesh )
	:	m_wpMesh	( p_pMesh	)
{
	m_mtxGlobalInverse.set_identity();
}

Skeleton :: ~Skeleton()
{
}

void Skeleton :: AddBone( BoneSPtr p_pBone )
{
	m_arrayBones.push_back( p_pBone );
}

//*************************************************************************************************
