#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, MeshGenerator )

namespace castor3d
{
	MeshGenerator::MeshGenerator( castor::String const & meshType )
		: m_meshType{ meshType }
	{
	}

	MeshGeneratorUPtr MeshGenerator::create()
	{
		return castor::makeUnique< MeshGenerator >( cuT( "custom" ) );
	}

	void MeshGenerator::generate( Mesh & mesh, Parameters const & parameters )
	{
		doGenerate( mesh, parameters );
		MeshPreparer::prepare( mesh, parameters );
		mesh.computeContainers();

		for ( auto & submesh : mesh )
		{
			mesh.getScene()->getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
		}
	}

	void MeshGenerator::computeNormals( Mesh & mesh, bool reverted )
	{
		for ( auto & submesh : mesh )
		{
			submesh->computeNormals( reverted );
		}
	}

	void MeshGenerator::doGenerate( CU_UnusedParam( Mesh &, mesh ), CU_UnusedParam( Parameters const &, parameters ) )
	{
	}
}
