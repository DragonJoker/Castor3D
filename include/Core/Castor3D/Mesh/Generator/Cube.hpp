/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CUBE_H___
#define ___C3D_CUBE_H___

#include "Castor3D/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cube representation
	\remark		The calling of cube is abusive because it has parametrable dimensions (width, height and depth)
	\~french
	\brief		Représentation d'un cube
	\remark		La dénomination "Cube" est un abus de langage car ses 3 dimensions sont paramétrables (largeur, hauteur et profondeur)
	*/
	class Cube
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cube();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cube();
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
		real m_height;
		real m_width;
		real m_depth;
		friend std::ostream & operator <<( std::ostream & o
			, Cube const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Opérateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Cube const & c )
	{
		return o << "Cube(" << c.m_height << "," << c.m_width << "," << c.m_depth << ")";
	}
}

#endif
