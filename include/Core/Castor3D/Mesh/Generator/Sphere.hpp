/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SPHERE_H___
#define ___C3D_SPHERE_H___

#include "Castor3D/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The sphere primitive
	\remark		This sphere has squared faces. It is built from a radius and a number of subsections
	\~french
	\brief		La primitive de sphère
	\remark		Cette sphère a des faces rectangulaires. Elle est construite à partir d'un rayon et d'un nombre de subdivisions
	*/
	class Sphere
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Sphere();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Sphere();
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
		real m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, Sphere const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Sphere const & c )
	{
		return o << "Sphere(" << c.m_nbFaces << "," << c.m_radius << ")";
	}
}

#endif
