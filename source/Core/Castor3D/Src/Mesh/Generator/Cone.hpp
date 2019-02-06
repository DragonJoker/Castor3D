/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CONE_H___
#define ___C3D_CONE_H___

#include "Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cone is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Le cône est une primitive basique, avec un nombre paramétrable de faces
	*/
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
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		//!\~english The number of faces	\~french Nombre de faces
		uint32_t m_nbFaces;
		//!\~english  The cone height	\~french La hauteur du cône
		real m_height;
		//!\~english  The cone radius	\~french Le rayon de la base
		real m_radius;
		friend std::ostream & operator <<( std::ostream & o
			, Cone const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o
		, Cone const & c )
	{
		return o << "Cone(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
