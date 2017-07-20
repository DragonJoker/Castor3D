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
	MeshGenerator::MeshGenerator( String const & p_meshType )
		: m_meshType{ p_meshType }
	{
	}

	MeshGenerator::~MeshGenerator()
	{
	}

	MeshGeneratorSPtr MeshGenerator::Create()
	{
		return std::make_shared< MeshGenerator >( cuT( "custom" ) );
	}

	void MeshGenerator::Generate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		DoGenerate( p_mesh, p_parameters );

		for ( auto submesh : p_mesh )
		{
			p_mesh.GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *submesh ) );
		}
	}

	void MeshGenerator::ComputeNormals( Mesh & p_mesh, bool p_reverted )
	{
		for ( auto submesh : p_mesh )
		{
			submesh->ComputeNormals( p_reverted );
		}
	}

	void MeshGenerator::DoGenerate( Mesh & CU_PARAM_UNUSED( p_mesh ), Parameters const &  CU_PARAM_UNUSED( p_parameters ) )
	{
	}
}
