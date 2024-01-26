/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Projection_H___
#define ___C3D_Projection_H___

#include "Castor3D/Model/Mesh/Generator/MeshGeneratorModule.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"
#include "Castor3D/Miscellaneous/Pattern.hpp"

namespace castor3d
{
	class Projection
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Projection();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorUPtr create();
		/**
		 *\~english
		 *\brief		Defines the pattern used to build the projection.
		 *\param[in]	pattern	The arc to project.
		 *\param[in]	axis	The projection axis.
		 *\param[in]	closed	Tells if the projection must be closed.
		 *\~french
		 *\brief		Définit l'arc utilisé pour construire la projection.
		 *\param[in]	pattern	L'arc à projeter.
		 *\param[in]	axis	L'axe de projection.
		 *\param[in]	closed	Dit si on doit fermer la projection.
		 */
		C3D_API void setPoints( Pattern< castor::Point3f > const & pattern
			, castor::Point3f const & axis
			, bool closed );

	private:
		C3D_API void doGenerate( Mesh & mesh
			, Parameters const & parameters )override;

	private:
		//Pattern< castor::Point3f > m_pattern{};
		//castor::Point3f m_axis{};
		//float m_depth{};
		//bool m_closed{};
		//uint32_t m_nbFaces{};
	};
}

#endif
