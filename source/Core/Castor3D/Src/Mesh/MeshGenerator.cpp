#include "MeshGenerator.hpp"

#include "Engine.hpp"

#include "Mesh.hpp"
#include "Submesh.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Scene.hpp"

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

	void MeshGenerator::doGenerate( Mesh & CU_PARAM_UNUSED( mesh ), Parameters const &  CU_PARAM_UNUSED( parameters ) )
	{
	}
}
