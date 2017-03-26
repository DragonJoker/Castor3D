#include "MeshGenerator.hpp"

#include "Engine.hpp"

#include "Mesh.hpp"
#include "Submesh.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshGenerator::MeshGenerator( MeshType p_eMeshType )
		: m_eMeshType( p_eMeshType )
	{
	}

	MeshGenerator::~MeshGenerator()
	{
	}

	MeshGeneratorSPtr MeshGenerator::Create()
	{
		return std::make_shared< MeshGenerator >( MeshType::eCustom );
	}

	void MeshGenerator::Generate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		DoGenerate( p_mesh, p_parameters );

		for ( auto l_submesh : p_mesh )
		{
			p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *l_submesh ) );
		}
	}

	void MeshGenerator::ComputeNormals( Mesh & p_mesh, bool p_reverted )
	{
		for ( auto l_submesh : p_mesh )
		{
			l_submesh->ComputeNormals( p_reverted );
		}
	}

	void MeshGenerator::DoGenerate( Mesh & CU_PARAM_UNUSED( p_mesh ), Parameters const &  CU_PARAM_UNUSED( p_parameters ) )
	{
	}
}
