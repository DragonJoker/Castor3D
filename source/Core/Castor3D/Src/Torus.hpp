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

#include "MeshGenerator.hpp"

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
	class Torus
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Torus();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Torus();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Generate
		 */
		C3D_API virtual void Generate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );
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

#endif
