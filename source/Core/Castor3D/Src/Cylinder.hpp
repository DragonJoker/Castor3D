﻿/*
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
#ifndef ___C3D_CYLINDER_H___
#define ___C3D_CYLINDER_H___

#include "MeshCategory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cylinder representation
	\remark		The cylinder is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Représentation d'un cylindre
	\remark		Un cylindre est une primitive basique avec un nombre de faces paramétrable
	*/
	class C3D_API Cylinder
		:	public MeshCategory
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_factory	The factory used to clone this MeshCategory
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_factory	La fabrique utilisée pour cloner cette MeshCategory
		 */
		Cylinder();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Cylinder();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		Cylinder
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un cylindre
		 */
		static MeshCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Generates the cylinder points and faces
		 *\~french
		 *\brief		Génère les points et faces du cylindre
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
		inline uint32_t GetFaceCount()const
		{
			return m_nbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves cylinder's height
		 *\~french
		 *\brief		Récupère la hauteur du cylindre
		 */
		inline real GetHeight()const
		{
			return m_height;
		}
		/**
		 *\~english
		 *\brief		Retrieves cylinder's radius
		 *\~french
		 *\brief		Récupère le rayon de la base du cylindre
		 */
		inline real GetRadius()const
		{
			return m_radius;
		}
		/**
		 *\~english
		 *\brief		Stream operator
		 *\~french
		 *\brief		Opérateur de flux
		 */
		inline friend std::ostream & operator <<( std::ostream & o, Cylinder const & c )
		{
			return o << "Cylinder(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
		}

	private:
		real m_height;
		real m_radius;
		uint32_t m_nbFaces;
	};
}

#endif
