#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"

namespace castor3d
{
	MeshAnimation::MeshAnimation( Mesh & mesh
		, castor::String const & name )
		: Animation{ *mesh.getEngine()
			, AnimationType::eMesh
			, mesh
			, name }
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh object )
	{
		m_submeshes.push_back( std::move( object ) );
	}
}
