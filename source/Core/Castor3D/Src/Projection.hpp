/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/

#ifndef ___C3D_PROJECTION_H___
#define ___C3D_PROJECTION_H___

#include "MeshCategory.hpp"

#include <Point.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
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
	class C3D_API Projection
		:	public MeshCategory
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Projection();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Projection();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A projection
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Une projection
		 */
		static MeshCategorySPtr Create();
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
		void SetPoints( Point3rPatternSPtr p_pPattern, Castor::Point3r const & p_vAxis, bool p_bClosed );
		/**
		 *\~english
		 *\brief		Generates the mesh points and faces
		 *\~french
		 *\brief		Génère les points et faces du mesh
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Modifies the mesh caracteristics then rebuild it
		 *\param[in]	p_arrayFaces		The new wanted mesh faces number
		 *\param[in]	p_arrayDimensions	The new wanted mesh dimensions
		 *\~french
		 *\brief		Modifie les caractéristiques du mesh et le reconstruit
		 *\param[in]	p_arrayFaces		Tableau contenant les nombres de faces
		 *\param[in]	p_arrayDimensions	Tableau contenant les dimensions du mesh
		 */
		virtual void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions );
		/**
		 *\~english
		 *\brief		Retrieves number of faces
		 *\~french
		 *\brief		Récupère le nombre de faces
		 */
		inline uint32_t	GetFaceCount()const
		{
			return m_uiNbFaces;
		}

	private:
		Point3rPatternSPtr m_pPattern;
		Castor::Point3r m_vAxis;
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

#pragma warning( pop )

#endif
