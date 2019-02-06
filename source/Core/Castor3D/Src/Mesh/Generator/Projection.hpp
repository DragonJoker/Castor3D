/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PROJECTION_H___
#define ___C3D_PROJECTION_H___

#include "Mesh/MeshGenerator.hpp"
#include "Miscellaneous/Pattern.hpp"

#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		26/08/2010
	\~english
	\brief		Projection mesh representation.
	\remark		The projection mesh is the projection of an arc over along axis on a given distance.
	\~french
	\brief		Représentation d'une projection.
	\remark		Ce type de mesh est la projection d'un arc selon un axe sur une distance donnée.
	*/
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
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Projection();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();
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
		C3D_API void setPoints( Pattern< castor::Point3r > const & pattern
			, castor::Point3r const & axis
			, bool closed );

	private:
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API void doGenerate( Mesh & mesh
			, Parameters const & parameters )override;

	private:
		Pattern< castor::Point3r > m_pattern;
		castor::Point3r m_axis;
		real m_depth;
		bool m_closed;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, Projection const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Projection const & c )
	{
		return o << "Projection( (" << c.m_axis[0] << "," << c.m_axis[1] << "," << c.m_axis[2] << ")," << c.m_depth << "," << c.m_closed << "," << c.m_nbFaces << ")";
	}
}

#endif
