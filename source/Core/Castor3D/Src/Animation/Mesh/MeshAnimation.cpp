#include "MeshAnimation.hpp"

#include "Engine.hpp"
#include "Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Mesh/Submesh.hpp"

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
