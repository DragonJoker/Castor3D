#include "Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

namespace castor3d
{
	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh )
		: castor::OwnedBy< MeshAnimation >{ animation }
		, m_submesh{ submesh }
	{
		if ( !m_submesh.hasComponent( MorphComponent::Name ) )
		{
			m_submesh.addComponent( MorphComponent::Name
				, std::make_shared< MorphComponent >( submesh ) );
		}

		m_component = m_submesh.getComponent< MorphComponent >();
	}
}
