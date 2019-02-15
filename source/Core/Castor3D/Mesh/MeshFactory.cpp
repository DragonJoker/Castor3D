#include "Castor3D/Mesh/MeshFactory.hpp"

#include "Castor3D/Mesh/Generator/Cone.hpp"
#include "Castor3D/Mesh/Generator/Cube.hpp"
#include "Castor3D/Mesh/Generator/Cylinder.hpp"
#include "Castor3D/Mesh/Generator/Icosahedron.hpp"
#include "Castor3D/Mesh/Generator/Plane.hpp"
#include "Castor3D/Mesh/Generator/Projection.hpp"
#include "Castor3D/Mesh/Generator/Sphere.hpp"
#include "Castor3D/Mesh/Generator/Torus.hpp"

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
