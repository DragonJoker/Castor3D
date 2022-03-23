#include "Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

namespace castor3d
{
	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh )
		: castor::OwnedBy< MeshAnimation >{ animation }
		, m_submesh{ submesh }
		, m_component{ std::make_shared< MorphComponent >( submesh ) }
	{
		m_submesh.addComponent( MorphComponent::Name, m_component );
	}
}
