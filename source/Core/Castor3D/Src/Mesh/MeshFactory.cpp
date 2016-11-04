#include "MeshFactory.hpp"

#include "Mesh/Generator/Cone.hpp"
#include "Mesh/Generator/Cube.hpp"
#include "Mesh/Generator/Cylinder.hpp"
#include "Mesh/Generator/Icosahedron.hpp"
#include "Mesh/Generator/Plane.hpp"
#include "Mesh/Generator/Projection.hpp"
#include "Mesh/Generator/Sphere.hpp"
#include "Mesh/Generator/Torus.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshFactory::MeshFactory()
		: Factory< MeshGenerator, MeshType >()
	{
		Register( MeshType::eCone, Cone::Create );
		Register( MeshType::eCube, Cube::Create );
		Register( MeshType::eCustom, MeshGenerator::Create );
		Register( MeshType::eCylinder, Cylinder::Create );
		Register( MeshType::eIcosahedron, Icosahedron::Create );
		Register( MeshType::ePlane, Plane::Create );
		Register( MeshType::eProjection, Projection::Create );
		Register( MeshType::eSphere, Sphere::Create );
		Register( MeshType::eTorus, Torus::Create );
	}

	MeshFactory::~MeshFactory()
	{
	}
}
