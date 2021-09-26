#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"

using namespace castor;

namespace castor3d
{
	MeshAnimation::MeshAnimation( Mesh & mesh
		, String const & name )
		: Animation{ AnimationType::eMesh
			, mesh
			, name }
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh object )
	{
		m_submeshes.push_back( std::move( object ) );
	}
}
