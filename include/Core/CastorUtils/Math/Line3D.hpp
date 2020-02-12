/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LINE_3D_H___
#define ___CASTOR_LINE_3D_H___

#include "CastorUtils/CastorUtils.hpp"

#include <cmath>

namespace castor
{
	/**
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
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	point	A point on the line
		 *\param[in]	slope	The line slope
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	point	Un point de la droite
		 *\param[in]	slope	La pente de la droite
		 */
		Line3D( Point< T, 3 > const & point, Point< T, 3 > const & slope )
			: m_origin( point )
			, m_slope( slope )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	point	A point on the line
		 *\param[in]	slope	The line slope
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	point	Un point de la droite
		 *\param[in]	slope	La pente de la droite
		 */
		static Line3D< T > fromPointAndSlope( Point< T, 3 > const & point, Point< T, 3 > const & slope )
		{
			return Line3D< T >( point, slope );
		}
		/**
		 *\~english
		 *\brief		Constructor from 2 points
		 *\param[in]	a, b	Two points on the line
		 *\~french
		 *\brief		Constructeur à partir de 2 points
		 *\param[in]	a, b	Deux points de la droite
		 */
		static Line3D< T > fromPoints( Point< T, 3 > const & a, Point< T, 3 > const & b )
		{
			return Line3D< T >( a, b - a );
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	line	The Line3D object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	line	L'objet Line3D à copier
		 */
		Line3D( Line3D const & line )
			:	m_origin( line.m_origin )
			,	m_slope( line.m_slope )
		{
		}
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	line	The Line3D object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	line	L'objet Line3D à déplacer
		 */
		Line3D( Line3D && line )
			:	m_origin( std::move( line.m_origin ) )
			,	m_slope( std::move( line.m_slope ) )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	line	The Line3D object to copy
		 *\return		A reference to this Line3D object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	line	L'objet Line3D à copier
		 *\return		Une référence sur cet objet Line3D
		 */
		Line3D & operator=( Line3D const & line )
		{
			m_origin = line.m_origin;
			m_slope = line.m_slope;
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	line	The Line3D object to move
		 *\return		A reference to this Line3D object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	line	L'objet Line3D à déplacer
		 *\return		Une référence sur cet objet Line3D
		 */
		Line3D & operator=( Line3D && line )
		{
			if ( this != &line )
			{
				m_origin = std::move( line.m_origin );
				m_slope = std::move( line.m_slope );
				line.m_origin	= Point< T, 3 >();
				line.m_slope	= Point< T, 3 >();
			}

			return * this;
		}
		/**
		 *\~english
		 *\brief		Computes intersection between this line and another one
		 *\param[in]	line	The other line
		 *\param[out]	point	Receives the intersection point
		 *\return		\p true if an intersection exists
		 *\~french
		 *\brief		Calcule l'intersection entre cette ligne et l'autre
		 *\param[in]	line	L'autre ligne
		 *\param[out]	point	Reçoit le point d'intersection
		 *\return		\p true si une intersection existe
		 */
		bool intersects( Line3D const & line, Point< T, 3 > & point )
		{
			bool result;
			double a = m_origin[0];
			double b = m_origin[1];
			double c = m_origin[2];
			double p = m_slope[0];
			double q = m_slope[1];
			double r = m_slope[2];
			double ap = line.m_origin[0];
			double bp = line.m_origin[1];
			double cp = line.m_origin[2];
			double pp = line.m_slope[0];
			double qp = line.m_slope[1];
			double rp = line.m_slope[2];
			double t;
			double tp;

			if ( std::abs( qp ) > std::numeric_limits< double >::epsilon()
				&& std::abs( p * qp - pp * q ) > std::numeric_limits< double >::epsilon() )
			{
				t = qp * ( ap + ( pp * ( b - bp ) / qp ) - a ) / ( p * qp - pp * q );
				tp = ( b + q * t - bp ) / qp;

				if ( std::abs( ( m_slope[2] * t + m_origin[2] ) - ( line.m_slope[2] * tp + line.m_origin[2] ) ) < std::numeric_limits< double >::epsilon() )
				{
					point[0] = T( m_slope[0] * t + m_origin[0] );
					point[1] = T( m_slope[1] * t + m_origin[1] );
					point[2] = T( m_slope[2] * t + m_origin[2] );
					result = true;
				}
				else
				{
					//invalid couple, no intersection
					result = false;
				}
			}
			else if ( std::abs( rp ) > std::numeric_limits< double >::epsilon()
				&& std::abs( p * rp - pp * r ) > std::numeric_limits< double >::epsilon() )
			{
				t = rp * ( ap + ( pp * ( c - cp ) / rp ) - a ) / ( p * rp - pp * r );
				tp = ( c + r * t - cp ) / rp;

				if ( std::abs( ( m_slope[2] * t + m_origin[2] ) - ( line.m_slope[2] * tp + line.m_origin[2] ) ) < std::numeric_limits< double >::epsilon() )
				{
					point[0] = T( m_slope[0] * t + m_origin[0] );
					point[1] = T( m_slope[1] * t + m_origin[1] );
					point[2] = T( m_slope[2] * t + m_origin[2] );
					result = true;
				}
				else
				{
					//invalid couple, no intersection
					result = false;
				}
			}
			else
			{
				// The 2 lines are parallel, no intersection
				result = false;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Tests if a point belongs to the line
		 *\param[in]	point	The point to test
		 *\return		\p true if the point belongs to the line
		 *\~french
		 *\brief		Teste si un point appartient à la ligne
		 *\param[in]	point	Le point à tester
		 *\return		\p true si le point appartient à la ligne
		 */
		bool isIn( Point< T, 3 > const & point )
		{
			return ( point[0] - m_origin[0] ) / m_slope[0] == T{}
				   && ( point[1] - m_origin[1] ) / m_slope[1] == T{}
				   && ( point[2] - m_origin[2] ) / m_slope[2] == T{};
		}

	public:
		//!\~english	The slope point.
		//!\~french		Le point de pente.
		Point< T, 3 > m_slope;
		//!\~english	The origin point.
		//!\~french		Le point d'origine.
		Point< T, 3 > m_origin;
	};
}

#endif
