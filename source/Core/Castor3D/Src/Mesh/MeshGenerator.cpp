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
	MeshGenerator::MeshGenerator( String const & p_meshType )
		: m_meshType{ p_meshType }
	{
	}

	MeshGenerator::~MeshGenerator()
	{
	}

	MeshGeneratorSPtr MeshGenerator::create()
	{
		return std::make_shared< MeshGenerator >( cuT( "custom" ) );
	}

	void MeshGenerator::generate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		doGenerate( p_mesh, p_parameters );

		for ( auto submesh : p_mesh )
		{
			p_mesh.getScene()->getListener().postEvent( MakeInitialiseEvent( *submesh ) );
		}
	}

	void MeshGenerator::computeNormals( Mesh & p_mesh, bool p_reverted )
	{
		for ( auto submesh : p_mesh )
		{
			submesh->computeNormals( p_reverted );
		}
	}

	void MeshGenerator::doGenerate( Mesh & CU_PARAM_UNUSED( p_mesh ), Parameters const &  CU_PARAM_UNUSED( p_parameters ) )
	{
	}
}
