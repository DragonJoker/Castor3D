/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Cube_H___
#define ___C3D_Cube_H___

#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
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
		float m_height;
		float m_width;
		float m_depth;
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
