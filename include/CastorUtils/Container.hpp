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
#ifndef ___Castor_Container___
#define ___Castor_Container___

#include "CastorUtils/Point.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Box container class
	\remark		A container will be a simple object which surrounds a graphic object (2D or 3D)
				<br />It can be a parallelepiped, a sphere or other.
	\~french
	\brief		Classe de conteneur boîte
	\remark		Un conteneur boîte est un simple objet encadrant un objet graphique (2D ou 3D)
				<br />Ce peut être un parallélépipède, une sphère ou autre.
	*/
	template< uint8_t Dimension >
	class ContainerBox
	{
	protected:
		typedef Castor::Policy< real > policy;
		//!\~english The center of the container	\~french	Le centre de ce conteneur
		Point< real, Dimension > m_ptCenter;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		ContainerBox()
			:	m_ptCenter	()
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_container	The container to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_container	Le conteneur à copier
		 */
		ContainerBox( ContainerBox const & p_container )
			:	m_ptCenter	( p_container.m_ptCenter	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_container	The container to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_container	Le conteneur à déplacer
		 */
		ContainerBox( ContainerBox && p_container )
			:	m_ptCenter	( std::move( p_container.m_ptCenter )	)
		{
			p_container.m_ptCenter = Point< real, Dimension >();
		}
		/**
		 *\~english
		 *\brief		Constructor from center
		 *\param[in]	p_ptCenter	The center
		 *\~french
		 *\brief		Constructeur à partir du centre
		 *\param[in]	p_ptCenter	Le centre
		 */
		ContainerBox( Point< real, Dimension > const & p_ptCenter )
			:	m_ptCenter	( p_ptCenter	)
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
		ContainerBox & operator =( ContainerBox const & p_container )
		{
			m_ptCenter = p_container.m_ptCenter;
			return * this;
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
		ContainerBox & operator =( ContainerBox && p_container )
		{
			if( this != &p_container )
			{
				m_ptCenter = std::move( p_container.m_ptCenter );
			}

			return * this;
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
		virtual bool IsWithin( Point< real, Dimension > const & p_v)=0;
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
		virtual bool IsOnLimits( Point< real, Dimension > const & p_v)=0;
		/**
		 *\~english
		 *\brief		Retrieves the center of this container
		 *\return		A constant reference to the center
		 *\~french
		 *\brief		Récupère le centre de ce conteneur
		 *\return		Une référence constante sur le centre
		 */
		inline Point< real, Dimension > const &	GetCenter()const { return m_ptCenter; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 2 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 2 dimensions
	*/
	typedef ContainerBox< 2 >	ContainerBox2D;
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 3 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 3 dimensions
	*/
	typedef ContainerBox< 3 >	ContainerBox3D;
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
	class CubeBox : public ContainerBox3D
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
			:	ContainerBox3D	()
			,	m_min			()
			,	m_max			()
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
			:	ContainerBox3D	( p_cube		)
			,	m_min			( p_cube.m_min	)
			,	m_max			( p_cube.m_max	)
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
			:	ContainerBox3D	( std::move( p_cube )		)
			,	m_min			( std::move( p_cube.m_min )	)
			,	m_max			( std::move( p_cube.m_max )	)
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
			:	ContainerBox3D	( p_min + (p_max - p_min) / real( 2.0 )	)
			,	m_min			( p_min									)
			,	m_max			( p_max									)
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

			if( this != &p_container )
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
			return (p_v[0] > m_min[0] && p_v[0] < m_max[0])
					&& (p_v[1] > m_min[1] && p_v[1] < m_max[1])
					&& (p_v[2] > m_min[2] && p_v[2] < m_max[2]);
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
			return ! IsWithin( p_v)
					&& (policy::equals( p_v[0], m_min[0])
						|| policy::equals( p_v[0], m_max[0])
						|| policy::equals( p_v[1], m_min[1])
						|| policy::equals( p_v[1], m_max[1])
						|| policy::equals( p_v[2], m_min[2])
						|| policy::equals( p_v[2], m_max[2]));
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
			m_ptCenter = (m_min + m_max) / real( 2.0 );
		}
		/**
		 *\~english
		 *\brief		Retrieves the min extent
		 *\return		A constant reference on the min extent
		 *\~french
		 *\brief		Récupère le point minimal
		 *\return		Une référence constante sur le point minimal
		 */
		inline Point3r const & GetMin()const { return m_min; }
		/**
		 *\~english
		 *\brief		Retrieves the max extent
		 *\return		A constant reference on the max extent
		 *\~french
		 *\brief		Récupère le point maximal
		 *\return		Une référence constante sur le point maximal
		 */
		inline Point3r const & GetMax()const { return m_max; }
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Sphere container class
	\~french
	\brief		Classe de conteneur sphérique
	*/
	class SphereBox : public ContainerBox3D
	{
	private:
		//!\~english The radius of the sphere	\~french Le rayon de la sphère
		real m_radius;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		SphereBox()
			:	ContainerBox3D	(	)
			,	m_radius		( 0	)
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_sphere	The sphere box to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_sphere	La sphere box à copier
		 */
		SphereBox( SphereBox const & p_sphere )
			:	ContainerBox3D	( p_sphere			)
			,	m_radius		( p_sphere.m_radius	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_sphere	The sphere box to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_sphere	La sphere box à déplacer
		 */
		SphereBox( SphereBox && p_sphere )
			:	ContainerBox3D	( std::move( p_sphere )				)
			,	m_radius		( std::move( p_sphere.m_radius )	)
		{
			p_sphere.m_radius = 0;
		}
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_center	The center of the sphere
		 *\param[in]	p_radius	The radius of the sphere
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_center	Le centre de la sphère
		 *\param[in]	p_radius	Le rayon de la sphère
		 */
		SphereBox( Point3r const & p_center, real p_radius )
			:	ContainerBox3D	( p_center	)
			,	m_radius		( p_radius	)
		{
		}
		/**
		 *\~english
		 *\brief		Constructor from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Constructeur à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		SphereBox( CubeBox const & p_box )
			:	ContainerBox3D	( p_box.GetCenter()											)
			,	m_radius		( real( point::distance( p_box.GetMax() - m_ptCenter ) )	)
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
		SphereBox & operator =( SphereBox const & p_container )
		{
			ContainerBox3D::operator =( p_container );
			m_radius = p_container.m_radius;
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
		SphereBox & operator =( SphereBox && p_container )
		{
			ContainerBox3D::operator =( std::move( p_container ) );

			if( this != &p_container )
			{
				m_radius = std::move( p_container.m_radius );
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box
		 *\param[in]	p_center	The center
		 *\param[in]	p_radius	The radius
		 *\~french
		 *\brief		Réinitialise la sphere box
		 *\param[in]	p_center	Le centre
		 *\param[in]	p_radius	Le rayon
		 */
		void Load( Point3r const & p_center, real p_radius )
		{
			m_ptCenter = p_center;
			m_radius = p_radius;
		}
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Réinitialise à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		void Load( CubeBox const & p_box )
		{
			m_ptCenter = p_box.GetCenter();
			m_radius = real( point::distance( p_box.GetMax() - m_ptCenter ) );
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
		virtual bool IsWithin( Point3r const & p_v ) { return point::distance( p_v - m_ptCenter ) < m_radius; }
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
		virtual bool IsOnLimits( Point3r const & p_v ) { return policy::equals( real( point::distance( p_v - m_ptCenter ) ), m_radius); }
		/**
		 *\~english
		 *\brief		Retrieves the radius
		 *\return		The radius
		 *\~french
		 *\brief		Récupère le rayon
		 *\return		Le rayon
		 */
		inline real	GetRadius()const { return m_radius; }
	};
}

#endif
