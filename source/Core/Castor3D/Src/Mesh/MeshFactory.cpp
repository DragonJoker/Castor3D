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
		: Factory< MeshGenerator, String >()
	{
		Register( cuT( "cone" ), Cone::Create );
		Register( cuT( "cube" ), Cube::Create );
		Register( cuT( "custom" ), MeshGenerator::Create );
		Register( cuT( "cylinder" ), Cylinder::Create );
		Register( cuT( "icosahedron" ), Icosahedron::Create );
		Register( cuT( "plane" ), Plane::Create );
		Register( cuT( "projection" ), Projection::Create );
		Register( cuT( "sphere" ), Sphere::Create );
		Register( cuT( "torus" ), Torus::Create );
	}

	MeshFactory::~MeshFactory()
	{
	}
}
