#include "Castor3D/Model/Mesh/MeshFactory.hpp"

#include "Castor3D/Model/Mesh/Generator/Cone.hpp"
#include "Castor3D/Model/Mesh/Generator/Cube.hpp"
#include "Castor3D/Model/Mesh/Generator/Cylinder.hpp"
#include "Castor3D/Model/Mesh/Generator/Icosahedron.hpp"
#include "Castor3D/Model/Mesh/Generator/Plane.hpp"
#include "Castor3D/Model/Mesh/Generator/Projection.hpp"
#include "Castor3D/Model/Mesh/Generator/Sphere.hpp"
#include "Castor3D/Model/Mesh/Generator/Torus.hpp"

CU_ImplementCUSmartPtr( castor3d, MeshFactory );

namespace castor3d
{
	MeshFactory::MeshFactory()
		: Factory< MeshGenerator, castor::String >{}
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
