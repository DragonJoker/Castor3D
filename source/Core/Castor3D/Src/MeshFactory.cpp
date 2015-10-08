#include "MeshFactory.hpp"

#include "Cone.hpp"
#include "Cube.hpp"
#include "Cylinder.hpp"
#include "Icosahedron.hpp"
#include "Plane.hpp"
#include "Projection.hpp"
#include "Sphere.hpp"
#include "Torus.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshFactory::MeshFactory()
		:	Factory< MeshGenerator, eMESH_TYPE >()
	{
		Initialise();
	}

	MeshFactory::~MeshFactory()
	{
	}

	void MeshFactory::Initialise()
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
}
