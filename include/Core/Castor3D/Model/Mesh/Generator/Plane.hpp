/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Plane_H___
#define ___C3D_Plane_H___

#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	class Plane
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Plane();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Plane();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();

	private:
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters );

	private:
		float m_depth;
		float m_width;
		uint32_t m_subDivisionsW;
		uint32_t m_subDivisionsD;
		friend std::ostream & operator <<( std::ostream & o
			, Plane const & c );
	};

	inline std::ostream & operator <<( std::ostream & o
		, Plane const & c )
	{
		return o << "Plane(" << c.m_depth << "," << c.m_width << "," << c.m_subDivisionsW << "," << c.m_subDivisionsD << ")";
	}
}

#endif
