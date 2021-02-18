/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Cylinder_H___
#define ___C3D_Cylinder_H___

#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	class Cylinder
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cylinder();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cylinder();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		float m_height;
		float m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o
			, Cylinder const & c );
	};

	inline std::ostream & operator <<( std::ostream & o
		, Cylinder const & c )
	{
		return o << "Cylinder(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
