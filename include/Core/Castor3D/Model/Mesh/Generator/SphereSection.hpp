/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SphereSection_H___
#define ___C3D_SphereSection_H___

#include "Castor3D/Model/Mesh/Generator/MeshGeneratorModule.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

#include <CastorUtils/Math/Angle.hpp>

namespace castor3d
{
	class SphereSection
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API SphereSection();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorUPtr create();

	private:
		C3D_API virtual void doGenerate( Mesh & mesh
			, Parameters const & parameters )override;

	private:
		float m_radius;
		castor::Angle m_angle;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, SphereSection const & c );
	};

	inline std::ostream & operator <<( std::ostream & o, SphereSection const & c )
	{
		return o << "SphereSection(" << c.m_nbFaces << "," << c.m_radius << "," << c.m_angle.degrees() << "°)";
	}
}

#endif
