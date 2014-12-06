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
#ifndef ___CASTOR_CUBE_BOX_H___
#define ___CASTOR_CUBE_BOX_H___

#include "ContainerBox.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Combo box container class
	\remark		A combo box is a box container
	\~french
	\brief		Classe de combo box
	\remark		Une combo box est un container boîte parallélépipédique
	*/
	class CubeBox
		: public ContainerBox3D
	{
	protected:
		//!\~english The min extent of the combo box	\~french Le point minimal de la combo box
		Point3r m_min;
		//!\~english The max extent of the combo box	\~french Le point maximal de la combo box
		Point3r m_max;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CubeBox()
			:	ContainerBox3D()
			,	m_min()
			,	m_max()
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_cube	The cube box to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_cube	La cube box à copier
		 */
		CubeBox( CubeBox const & p_cube )
			:	ContainerBox3D( p_cube	)
			,	m_min( p_cube.m_min	)
			,	m_max( p_cube.m_max	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_cube	The cube box to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_cube	La cube box à déplacer
		 */
		CubeBox( CubeBox && p_cube )
			:	ContainerBox3D( std::move( p_cube )	)
			,	m_min( std::move( p_cube.m_min )	)
			,	m_max( std::move( p_cube.m_max )	)
		{
			p_cube.m_min = Point3r( 0, 0, 0 );
			p_cube.m_max = Point3r( 0, 0, 0 );
		}
		/**
		 *\~english
		 *\brief		Constructor from min and max extents
		 *\param[in]	p_min	The min extent
		 *\param[in]	p_max	The max extent
		 *\~french
		 *\brief		Constructeur à partir des points min et max
		 *\param[in]	p_min	Le point minimal
		 *\param[in]	p_max	Le point maximal
		 */
		CubeBox( Point3r const & p_min, Point3r const & p_max )
			:	ContainerBox3D( p_min + ( p_max - p_min ) / real( 2.0 )	)
			,	m_min( p_min	)
			,	m_max( p_max	)
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		CubeBox & operator =( CubeBox const & p_container )
		{
			ContainerBox3D::operator =( p_container );
			m_min = p_container.m_min;
			m_max = p_container.m_max;
			return *this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		CubeBox & operator =( CubeBox && p_container )
		{
			ContainerBox3D::operator =( std::move( p_container ) );

			if ( this != &p_container )
			{
				m_min = std::move( p_container.m_min );
				m_max = std::move( p_container.m_max );
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Tests if a vertex is within the container, id est inside it but not on it's limits
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is within the container, false if not
		 *\~french
		 *\brief		Teste si un point est contenu dans le container (mais pas à la limite)
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est dans le container
		 */
		virtual bool IsWithin( Point3r const & p_v )
		{
			return ( p_v[0] > m_min[0] && p_v[0] < m_max[0] )
				   && ( p_v[1] > m_min[1] && p_v[1] < m_max[1] )
				   && ( p_v[2] > m_min[2] && p_v[2] < m_max[2] );
		}
		/**
		 *\~english
		 *\brief		Tests if a vertex is on the limits of this container, and not within
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is on the limits of the container, false if not
		 *\~french
		 *\brief		Teste si un point est sur la limite du container, et pas dedans
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est sur la limite
		 */
		virtual bool IsOnLimits( Point3r const & p_v )
		{
			return ! IsWithin( p_v )
				   && ( policy::equals( p_v[0], m_min[0] )
						|| policy::equals( p_v[0], m_max[0] )
						|| policy::equals( p_v[1], m_min[1] )
						|| policy::equals( p_v[1], m_max[1] )
						|| policy::equals( p_v[2], m_min[2] )
						|| policy::equals( p_v[2], m_max[2] ) );
		}
		/**
		 *\~english
		 *\brief		Reinitialises the combo box to the given limits
		 *\param[in]	p_ptMin	The min extent
		 *\param[in]	p_ptMax	The max extent
		 *\~french
		 *\brief		Réinitialise la boîte aux limites données
		 *\param[in]	p_ptMin	Le point minimal
		 *\param[in]	p_ptMax	Le point maximal
		 */
		void Load( Point3r const & p_ptMin, Point3r const & p_ptMax )
		{
			m_min = p_ptMin;
			m_max = p_ptMax;
			m_ptCenter = ( m_min + m_max ) / real( 2.0 );
		}
		/**
		 *\~english
		 *\brief		Retrieves the min extent
		 *\return		A constant reference on the min extent
		 *\~french
		 *\brief		Récupère le point minimal
		 *\return		Une référence constante sur le point minimal
		 */
		inline Point3r const & GetMin()const
		{
			return m_min;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max extent
		 *\return		A constant reference on the max extent
		 *\~french
		 *\brief		Récupère le point maximal
		 *\return		Une référence constante sur le point maximal
		 */
		inline Point3r const & GetMax()const
		{
			return m_max;
		}
	};
}

#endif
