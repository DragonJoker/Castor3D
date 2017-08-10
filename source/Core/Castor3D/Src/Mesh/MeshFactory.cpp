#include "MeshFactory.hpp"

#include "Mesh/Generator/Cone.hpp"
#include "Mesh/Generator/Cube.hpp"
#include "Mesh/Generator/Cylinder.hpp"
#include "Mesh/Generator/Icosahedron.hpp"
#include "Mesh/Generator/Plane.hpp"
#include "Mesh/Generator/Projection.hpp"
#include "Mesh/Generator/Sphere.hpp"
#include "Mesh/Generator/Torus.hpp"

using namespace castor;

namespace castor3d
{
	MeshFactory::MeshFactory()
		: Factory< MeshGenerator, String >()
	{
		registerType( cuT( "cone" ), Cone::create );
		registerType( cuT( "cube" ), Cube::create );
		registerType( cuT( "custom" ), MeshGenerator::create );
		registerType( cuT( "cylinder" ), Cylinder::create );
		registerType( cuT( "icosahedron" ), Icosahedron::create );
		registerType( cuT( "plane" ), Plane::create );
		registerType( cuT( "projection" ), Projection::create );
		registerType( cuT( "sphere" ), Sphere::create );
		registerType( cuT( "torus" ), Torus::create );
	}

	MeshFactory::~MeshFactory()
	{
	}
}
