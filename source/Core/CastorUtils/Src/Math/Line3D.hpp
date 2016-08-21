/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_LINE_3D_H___
#define ___CASTOR_LINE_3D_H___

#include "CastorUtils.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/08/2010
	\~english
	\brief		3D line equation
	\remark		A slope and an origin
				<br />x = m_slope[0] * t + m_origin[0]
				<br />y = m_slope[1] * t + m_origin[1]
				<br />z = m_slope[2] * t + m_origin[2]
	\~french
	\brief		Equation d'une droite 3D
	\remark		Une pente et une origine
				<br />x = m_slope[0] * t + m_origin[0]
				<br />y = m_slope[1] * t + m_origin[1]
				<br />z = m_slope[2] * t + m_origin[2]
	*/
	template< typename T >
	class Line3D
	{
	private:
		typedef Castor::Policy< T > policy;

	private:
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	p_point	A point on the line
		 *\param[in]	p_ptSlope	The line slope
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	p_ptA	Un point de la droite
		 *\param[in]	p_ptB	La pente de la droite
		 */
		Line3D( Point< T, 3 > const & p_point, Point< T, 3 > const & p_ptSlope )
			:	m_origin( p_point )
			,	m_slope( p_ptSlope )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	p_point	A point on the line
		 *\param[in]	p_ptSlope	The line slope
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	p_point	Un point de la droite
		 *\param[in]	p_ptSlope	La pente de la droite
		 */
		static Line3D< T > FromPointAndSlope( Point< T, 3 > const & p_point, Point< T, 3 > const & p_ptSlope )
		{
			return Line3D< T >( p_point, p_ptSlope );
		}
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	p_ptA, p_ptB	Two points on the line
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	p_ptA, p_ptB	Deux points de la droite
		 */
		static Line3D< T > FromPoints( Point< T, 3 > const & p_ptA, Point< T, 3 > const & p_ptB )
		{
			return Line3D< T >( p_ptA, p_ptB - p_ptA );
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_line	The Line3D object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_line	L'objet Line3D à copier
		 */
		Line3D( Line3D const & p_line )
			:	m_origin( p_line.m_origin )
			,	m_slope( p_line.m_slope )
		{
		}
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_line	The Line3D object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_line	L'objet Line3D à déplacer
		 */
		Line3D( Line3D && p_line )
			:	m_origin( std::move( p_line.m_origin ) )
			,	m_slope( std::move( p_line.m_slope ) )
		{
			p_line.m_origin	= Point< T, 3 >();
			p_line.m_slope	= Point< T, 3 >();
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_line	The Line3D object to copy
		 *\return		A reference to this Line3D object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_line	L'objet Line3D à copier
		 *\return		Une référence sur cet objet Line3D
		 */
		Line3D & operator =( Line3D const & p_line )
		{
			m_origin	= p_line.m_origin;
			m_slope		= p_line.m_slope;
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_line	The Line3D object to move
		 *\return		A reference to this Line3D object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_line	L'objet Line3D à déplacer
		 *\return		Une référence sur cet objet Line3D
		 */
		Line3D & operator =( Line3D && p_line )
		{
			if ( this != &p_line )
			{
				m_origin	= std::move( p_line.m_origin );
				m_slope		= std::move( p_line.m_slope );
				p_line.m_origin	= Point< T, 3 >();
				p_line.m_slope	= Point< T, 3 >();
			}

			return * this;
		}
		/**
		 *\~english
		 *\brief		Computes intersection between this line and another one
		 *\param[in]	p_line	The other line
		 *\param[out]	p_point	Receives the intersection point
		 *\return		\p true if an intersection exists
		 *\~french
		 *\brief		Calcule l'intersection entre cette ligne et l'autre
		 *\param[in]	p_line	L'autre ligne
		 *\param[out]	p_point	Reçoit le point d'intersection
		 *\return		\p true si une intersection existe
		 */
		bool Intersects( Line3D const & p_line, Point< T, 3 > & p_point )
		{
			bool l_return;
			double a = m_origin[0];
			double b = m_origin[1];
			double c = m_origin[2];
			double p = m_slope[0];
			double q = m_slope[1];
			double r = m_slope[2];
			double ap = p_line.m_origin[0];
			double bp = p_line.m_origin[1];
			double cp = p_line.m_origin[2];
			double pp = p_line.m_slope[0];
			double qp = p_line.m_slope[1];
			double rp = p_line.m_slope[2];
			double t;
			double tp;

			if ( std::abs( qp ) > std::numeric_limits< double >::epsilon() && std::abs( p * qp - pp * q ) > std::numeric_limits< double >::epsilon() )
			{
				t = qp * ( ap + ( pp * ( b - bp ) / qp ) - a ) / ( p * qp - pp * q );
				tp = ( b + q * t - bp ) / qp;

				if ( std::abs( ( m_slope[2] * t + m_origin[2] ) - ( p_line.m_slope[2] * tp + p_line.m_origin[2] ) ) < std::numeric_limits< double >::epsilon() )
				{
					p_point[0] = T( m_slope[0] * t + m_origin[0] );
					p_point[1] = T( m_slope[1] * t + m_origin[1] );
					p_point[2] = T( m_slope[2] * t + m_origin[2] );
					l_return = true;
				}
				else
				{
					//invalid couple, no intersection
					l_return = false;
				}
			}
			else if ( std::abs( rp ) > std::numeric_limits< double >::epsilon() && std::abs( p * rp - pp * r ) > std::numeric_limits< double >::epsilon() )
			{
				t = rp * ( ap + ( pp * ( c - cp ) / rp ) - a ) / ( p * rp - pp * r );
				tp = ( c + r * t - cp ) / rp;

				if ( std::abs( ( m_slope[2] * t + m_origin[2] ) - ( p_line.m_slope[2] * tp + p_line.m_origin[2] ) ) < std::numeric_limits< double >::epsilon() )
				{
					p_point[0] = T( m_slope[0] * t + m_origin[0] );
					p_point[1] = T( m_slope[1] * t + m_origin[1] );
					p_point[2] = T( m_slope[2] * t + m_origin[2] );
					l_return = true;
				}
				else
				{
					//invalid couple, no intersection
					l_return = false;
				}
			}
			else
			{
				// The 2 lines are parallel, no intersection
				l_return = false;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Tests if a point belongs to the line
		 *\param[in]	p_point	The point to test
		 *\return		\p true if the point belongs to the line
		 *\~french
		 *\brief		Teste si un point appartient à la ligne
		 *\param[in]	p_point	Le point à tester
		 *\return		\p true si le point appartient à la ligne
		 */
		bool IsIn( Point< T, 3 > const & p_point )
		{
			return policy::is_null( ( p_point[0] - m_origin[0] ) / m_slope[0] )
				   && policy::is_null( ( p_point[1] - m_origin[1] ) / m_slope[1] )
				   && policy::is_null( ( p_point[2] - m_origin[2] ) / m_slope[2] );
		}

	public:
		//!\~english The slope point	\~french Le point de pente
		Point< T, 3 > m_slope;
		//!\~english The origin point	\~french Le point d'origine
		Point< T, 3 > m_origin;
	};
}

#endif
