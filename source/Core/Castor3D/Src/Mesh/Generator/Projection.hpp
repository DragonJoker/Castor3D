/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PROJECTION_H___
#define ___C3D_PROJECTION_H___

#include "Mesh/MeshGenerator.hpp"

#include <Math/Point.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		26/08/2010
	\~english
	\brief		Projection mesh representation
	\remark		The projection mesh is the projection of an arc over along axis on a given distance
	\~french
	\brief		Représentation d'une projection
	\remark		Ce type de mesh est la projection d'un arc selon un axe sur une distance donnée
	*/
	class Projection
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Projection();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Projection();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorSPtr create();
		/**
		 *\~english
		 *\brief		Defines the pattern used to build the projection
		 *\param[in]	p_pPattern	The arc to project
		 *\param[in]	p_vAxis		The projection axis
		 *\param[in]	p_bClosed	Tells if the projection must be closed
		 *\~french
		 *\brief		Définit l'arc utilisé pour construire la projection
		 *\param[in]	p_pPattern	L'arc à projeter
		 *\param[in]	p_vAxis		L'axe de projection
		 *\param[in]	p_bClosed	Dit si on doit fermer la projection
		 */
		C3D_API void setPoints( Point3rPatternSPtr p_pPattern, castor::Point3r const & p_vAxis, bool p_bClosed );

	private:
		/**
		*\copydoc		castor3d::MeshGenerator::doGenerate
		*/
		C3D_API virtual void doGenerate( Mesh & p_mesh, Parameters const & p_parameters );

	private:
		Point3rPatternSPtr m_pPattern;
		castor::Point3r m_vAxis;
		real m_fDepth;
		bool m_bClosed;
		uint32_t m_uiNbFaces;
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
		return o << "Projection( (" << c.m_vAxis[0] << "," << c.m_vAxis[1] << "," << c.m_vAxis[2] << ")," << c.m_fDepth << "," << c.m_bClosed << "," << c.m_uiNbFaces << ")";
	}
}

#endif
