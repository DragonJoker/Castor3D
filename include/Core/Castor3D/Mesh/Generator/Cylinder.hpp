/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CYLINDER_H___
#define ___C3D_CYLINDER_H___

#include "Castor3D/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cylinder representation
	\remark		The cylinder is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Représentation d'un cylindre
	\remark		Un cylindre est une primitive basique avec un nombre de faces paramétrable
	*/
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
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		float m_height;
		float m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o
			, Cylinder const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Opérateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o
		, Cylinder const & c )
	{
		return o << "Cylinder(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
