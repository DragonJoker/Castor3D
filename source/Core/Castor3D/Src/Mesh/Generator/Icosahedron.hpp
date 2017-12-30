/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ICOSAHEDRON_H___
#define ___C3D_ICOSAHEDRON_H___

#include "Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The icosahedron representation
	\remark		An icosahedron is a geometric polygon constituted by 20 equilateral triangles.
				<br />This mesh is used to build a sphere with triangular faces.
	\~french
	\brief		Représentation d'un icosaèdre
	\remark		Un icosaèdre est un polygône constitué de 20 triangles équilatéraux
				<br />Ce mesh est utilisé pour construire des sphères à faces triangulaires
	*/
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
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Icosahedron();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API virtual void doGenerate( Mesh & p_mesh, Parameters const & p_parameters );

	private:
		real m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, Icosahedron const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Icosahedron const & c )
	{
		return o << "Icosahedron(" << c.m_nbFaces << "," << c.m_radius << ")";
	}
}

#endif
