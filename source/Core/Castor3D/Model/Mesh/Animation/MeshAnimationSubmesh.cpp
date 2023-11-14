#include "Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

CU_ImplementSmartPtr( castor3d, MeshAnimationSubmesh )

namespace castor3d
{
	MeshAnimationSubmesh::MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh )
		: castor::OwnedBy< MeshAnimation >{ animation }
		, m_submesh{ submesh }
	{
		CU_Require( m_submesh.hasComponent( MorphComponent::TypeName ) );
		m_component = m_submesh.getComponent< MorphComponent >();
	}
}
