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
#ifndef ___C3D_Pave___
#define ___C3D_Pave___

#include "Mesh.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
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
	class C3D_API Cube : public MeshCategory 
	{
	private:
		real m_height;
		real m_width;
		real m_depth;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Cube();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Cube();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A MeshCategory
		 *\~french
		 *\brief		Fonction de création, utilisée par la Factory
		 *\return		Un MeshCategory
		 */
		static MeshCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Generates the cube points and faces
		 *\~french
		 *\brief		Génère les points et faces du cube
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Modifies the mesh caracteristics then rebuild it
		 *\param[in]	p_arrayFaces		The new wanted mesh faces number (unused)
		 *\param[in]	p_arrayDimensions	The new wanted mesh dimensions
		 *\~french
		 *\brief		Modifie les caractéristiques du mesh et le reconstruit
		 *\param[in]	p_arrayFaces		Tableau contenant les nombres de faces (inutilisé pour Cube)
		 *\param[in]	p_arrayDimensions	Tableau contenant les dimensions du mesh
		 */
		virtual void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions);
		/**
		 *\~english
		 *\brief		Retrieves the cube height
		 *\~french
		 *\brief		Récupère la hauteur du cube
		 */
		inline real GetHeight()const {return m_height;}
		/**
		 *\~english
		 *\brief		Retrieves the cube width
		 *\~french
		 *\brief		Récupère la largeur du cube
		 */
		inline real GetWidth()const {return m_width;}
		/**
		 *\~english
		 *\brief		Retrieves the cube depth
		 *\~french
		 *\brief		Récupère la profondeur du cube
		 */
		inline real GetDepth()const {return m_depth;}
		/**
		 *\~english
		 *\brief		Stream operator
		 *\~french
		 *\brief		Opérateur de flux
		 */
		inline friend std::ostream & operator <<( std::ostream & o, Cube const & c) { return o << "Pave(" << c.m_height << "," << c.m_width << "," << c.m_depth << ")";}
	};
}

#pragma warning( pop )

#endif
