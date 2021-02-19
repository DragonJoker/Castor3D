/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Cone_H___
#define ___C3D_Cone_H___

#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	class Cone
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cone();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cone();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		//!\~english The number of faces	\~french Nombre de faces
		uint32_t m_nbFaces;
		//!\~english  The cone height	\~french La hauteur du cône
		float m_height;
		//!\~english  The cone radius	\~french Le rayon de la base
		float m_radius;
		friend std::ostream & operator <<( std::ostream & o
			, Cone const & c );
	};

	inline std::ostream & operator <<( std::ostream & o
		, Cone const & c )
	{
		return o << "Cone(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
