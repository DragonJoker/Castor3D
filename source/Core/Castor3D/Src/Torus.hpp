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
#ifndef ___C3D_TORUS_H___
#define ___C3D_TORUS_H___

#include "MeshCategory.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The torus representation
	\remark		A torus is an ellipse performing an horizontal ellipse.
				<br />The original ellipse will be called internal and has its own radius and number of subsections
	\~french
	\brief		Représentation d'un torre
	\remark		Un torre est une ellipse qui parcourt une ellipse
				<br />L'ellipse originale sera appelée interne et possède son propre rayon et nombre de sections
	*/
	class C3D_API Torus
		:	public MeshCategory
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Torus();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Torus();
		/**
		 *\~english
		 *\brief		Generates the mesh points and faces
		 *\~french
		 *\brief		Génère les points et faces du mesh
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A torus
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Un torre
		 */
		static MeshCategorySPtr Create();
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
		 *\brief		Retrieves the internal ellipse faces number
		 *\~french
		 *\brief		Récupère le nombre de face de l'ellipse interne
		 */
		inline uint32_t	GetInternalNbFaces()const
		{
			return m_uiInternalNbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the external ellipse faces number
		 *\~french
		 *\brief		Récupère le nombre de face de l'ellipse externe
		 */
		inline uint32_t	GetExternalNbFaces()const
		{
			return m_uiExternalNbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the internal ellipse radius
		 *\~french
		 *\brief		Récupère le rayon de l'ellipse interne
		 */
		inline real GetInternalRadius()const
		{
			return m_rInternalRadius;
		}
		/**
		 *\~english
		 *\brief		Retrieves the external ellipse radius
		 *\~french
		 *\brief		Récupère le rayon de l'ellipse externe
		 */
		inline real GetExternalRadius()const
		{
			return m_rExternalRadius;
		}

	private:
		real m_rInternalRadius;
		real m_rExternalRadius;
		uint32_t m_uiInternalNbFaces;
		uint32_t m_uiExternalNbFaces;
		friend std::ostream & operator <<( std::ostream & o, Torus const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Torus const & c )
	{
		return o << "Torus(" << c.m_uiInternalNbFaces << "," << c.m_uiExternalNbFaces << "," << c.m_rInternalRadius << "," << c.m_rExternalRadius << ")";
	}
}

#pragma warning( pop )

#endif
