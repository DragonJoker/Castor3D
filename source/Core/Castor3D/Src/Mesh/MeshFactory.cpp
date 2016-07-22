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
		: Factory< MeshGenerator, eMESH_TYPE >()
	{
		Register( eMESH_TYPE_CONE, Cone::Create );
		Register( eMESH_TYPE_CUBE, Cube::Create );
		Register( eMESH_TYPE_CUSTOM, MeshGenerator::Create );
		Register( eMESH_TYPE_CYLINDER, Cylinder::Create );
		Register( eMESH_TYPE_ICOSAHEDRON, Icosahedron::Create );
		Register( eMESH_TYPE_PLANE, Plane::Create );
		Register( eMESH_TYPE_PROJECTION, Projection::Create );
		Register( eMESH_TYPE_SPHERE, Sphere::Create );
		Register( eMESH_TYPE_TORUS, Torus::Create );
	}

	MeshFactory::~MeshFactory()
	{
	}
}
