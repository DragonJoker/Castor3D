/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Icosahedron_H___
#define ___C3D_Icosahedron_H___

#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	class Icosahedron
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Icosahedron();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters )override;

	private:
		float m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o
			, Icosahedron const & c );
	};

	inline std::ostream & operator <<( std::ostream & o, Icosahedron const & c )
	{
		return o << "Icosahedron(" << c.m_nbFaces << "," << c.m_radius << ")";
	}
}

#endif
