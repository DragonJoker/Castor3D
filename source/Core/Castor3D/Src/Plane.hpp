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
#ifndef ___C3D_PLANE_H___
#define ___C3D_PLANE_H___

#include "MeshCategory.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The plane representation
	\remark		A plane can be subdivided in width and in height.
	\~french
	\brief		Représentation d'un plan
	\remark		Un plan peut être subdivisé en hauteur et en largeur
	*/
	class C3D_API Plane
		: public MeshCategory
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Plane();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Plane();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A plane
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Un plan
		 */
		static MeshCategorySPtr Create();
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
		 *\brief		Retrieves the plane height
		 *\~french
		 *\brief		Récupère la hauteur du plan
		 */
		inline real GetHeight()const
		{
			return m_depth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane width
		 *\~french
		 *\brief		Récupère la largeur du plan
		 */
		inline real GetWidth()const
		{
			return m_width;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane width subdivisions
		 *\~french
		 *\brief		Récupère le nombre de subdivisions en largeur du plan
		 */
		inline uint32_t	GetSubDivisionsX()const
		{
			return m_subDivisionsW;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane height subdivisions
		 *\~french
		 *\brief		Récupère le nombre de subdivisions en hauteur du plan
		 */
		inline uint32_t	GetSubDivisionsY()const
		{
			return m_subDivisionsD;
		}
		/**
		 *\~english
		 *\brief		Stream operator
		 *\~french
		 *\brief		Operateur de flux
		 */
		inline friend std::ostream & operator <<( std::ostream & o, Plane const & c )
		{
			return o << "Plane(" << c.m_depth << "," << c.m_width << "," << c.m_subDivisionsW << "," << c.m_subDivisionsD << ")";
		}

	private:
		real		m_depth;
		real		m_width;
		uint32_t	m_subDivisionsW;
		uint32_t	m_subDivisionsD;
	};
}

#pragma warning( pop )

#endif
