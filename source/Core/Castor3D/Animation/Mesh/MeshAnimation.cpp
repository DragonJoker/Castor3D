#include "Castor3D/Animation/Mesh/MeshAnimation.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Mesh/Submesh.hpp"

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

	MeshAnimation::~MeshAnimation()
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh && object )
	{
		m_submeshes.push_back( std::move( object ) );
	}
}
