#include "MeshAnimationSubmesh.hpp"

#include "MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/MorphComponent.hpp"

using namespace castor;

namespace castor3d
{
	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh )
		: OwnedBy< MeshAnimation >{ animation }
		, m_submesh{ submesh }
		, m_component{ std::make_shared< MorphComponent >( submesh ) }
	{
		m_submesh.addComponent( MorphComponent::Name, m_component );
	}

	MeshAnimationSubmesh::~MeshAnimationSubmesh()
	{
	}
}
