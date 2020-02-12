#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	MeshGenerator::MeshGenerator( String const & meshType )
		: m_meshType{ meshType }
	{
	}

	MeshGenerator::~MeshGenerator()
	{
	}

	MeshGeneratorSPtr MeshGenerator::create()
	{
		return std::make_shared< MeshGenerator >( cuT( "custom" ) );
	}

	void MeshGenerator::generate( Mesh & mesh, Parameters const & parameters )
	{
		doGenerate( mesh, parameters );

		for ( auto submesh : mesh )
		{
			mesh.getScene()->getListener().postEvent( makeInitialiseEvent( *submesh ) );
		}
	}

	void MeshGenerator::computeNormals( Mesh & mesh, bool reverted )
	{
		for ( auto submesh : mesh )
		{
			submesh->computeNormals( reverted );
		}
	}

	void MeshGenerator::doGenerate( Mesh & CU_UnusedParam( mesh ), Parameters const &  CU_UnusedParam( parameters ) )
	{
	}
}
