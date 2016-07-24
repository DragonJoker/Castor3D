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
#ifndef ___CASTOR_ANGLE_H___
#define ___CASTOR_ANGLE_H___

#include "CastorUtils.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Angle helper class
	\remark		Used to manage angles without taking care of degrees or radians
	\~french
	\brief		Classe d'angle
	\remark		Utilisée pour gérer les angles sans avoir à se préoccuper des convertions degré / radian / gradient
	*/
	class Angle
	{
	public:
		//!\~english Radian to degree conversion constant	\~french Constante de conversion de radian vers degré
		CU_API static const double RadianToDegree;
		//!\~english Radian to gradient conversion constant	\~french Constante de conversion de radian vers gradient
		CU_API static const double RadianToGrad;
		//!\~english Degree to radian conversion constant	\~french Constante de conversion de degré vers radian
		CU_API static const double DegreeToRadian;
		//!\~english Degree to gradient conversion constant	\~french Constante de conversion de degré vers gradient
		CU_API static const double DegreeToGrad;
		//!\~english Gradient to degree conversion constant	\~french Constante de conversion de gradient vers degré
		CU_API static const double GradToDegree;
		//!\~english Gradient to radian conversion constant	\~french Constante de conversion de gradient vers radian
		CU_API static const double GradToRadian;
		//!\~english Pi constant, expressed in real	\~french Constante pi exprimée en real
		CU_API static const double Pi;
		//!\~english Pi * 2 constant	\~french Constante pi * 2
		CU_API static const double PiMult2;
		//!\~english Pi / 2 constant	\~french Constante pi / 2
		CU_API static const double PiDiv2;

	private:
		/**
		 *\~english
		 *\brief		Specified Constructor
		 *\remarks		Private so specified construction only available through named ctors
		 *\~french
		 *\brief		Constructeur spécifié
		 *\remarks		Privé afin que la construction spécifiée ne soit accessible qu'à partir des constructeurs nommés
		 */
		template< typename T >
		explicit Angle( T p_radians )
			: m_radians( double( p_radians ) )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Named constructor, from a degrees angle
		 *\param[in]	p_degrees	The angle in degrees
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en degrés
		 *\param[in]	p_degrees	L'angle, exprimé en degrés
		 *\return		L'angle construit
		 */
		template< typename T >
		static Angle from_degrees( T p_degrees )
		{
			return Angle( p_degrees * DegreeToRadian );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from a radians angle
		 *\param[in]	p_radians	The angle in radians
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en radians
		 *\param[in]	p_radians	L'angle, exprimé en radians
		 *\return		L'angle construit
		 */
		template< typename T >
		static Angle from_radians( T p_radians )
		{
			return Angle( p_radians * 1.0 );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from a gradients angle
		 *\param[in]	p_grads	The angle in gradients
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en gradients
		 *\param[in]	p_grads	L'angle, exprimé en gradients
		 *\return		L'angle construit
		 */
		template< typename T >
		static Angle from_grads( T p_grads )
		{
			return Angle( p_grads * GradToRadian );
		}
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CU_API Angle();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Angle();
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_rhs	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_rhs	L'objet à copier
		 */
		CU_API Angle( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_rhs	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_rhs	L'objet à déplacer
		 */
		CU_API Angle( Angle && p_rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_rhs	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_rhs	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		CU_API Angle & operator=( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_rhs	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_rhs	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		CU_API Angle & operator=( Angle && p_rhs );
		/**
		 *\~english
		 *\brief		Conversion to degrees
		 *\return		Angle value, in degrees
		 *\~french
		 *\brief		Conversion en degrés
		 *\return		La valeur de l'angle, en degrés
		 */
		CU_API double degrees()const;
		/**
		 *\~english
		 *\brief		Conversion to radians
		 *\return		Angle value, in radians
		 *\~french
		 *\brief		Conversion en radians
		 *\return		La valeur de l'angle, en radians
		 */
		CU_API double radians()const;
		/**
		 *\~english
		 *\brief		Conversion to gradients
		 *\return		Angle value, in gradients
		 *\~french
		 *\brief		Conversion en gradients
		 *\return		La valeur de l'angle, en gradients
		 */
		CU_API double grads()const;
		/**
		 *\~english
		 *\brief		Sets this angle value from degrees
		 *\param[in]	p_rAngle	Angle in degrees
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de degrés
		 *\param[in]	p_rAngle	L'angle exprimé en degrés
		 */
		CU_API void degrees( double p_rAngle );
		/**
		 *\~english
		 *\brief		Sets this angle value from radians
		 *\param[in]	p_rAngle	Angle in radians
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de radians
		 *\param[in]	p_rAngle	L'angle exprimé en radians
		 */
		CU_API void radians( double p_rAngle );
		/**
		 *\~english
		 *\brief		Sets this angle value from gradients
		 *\param[in]	p_rAngle	Angle in gradients
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de gradients
		 *\param[in]	p_rAngle	L'angle exprimé en gradients
		 */
		CU_API void grads( double p_rAngle );
		/**
		 *\~english
		 *\brief		Trigonometric cosine
		 *\return		Cosine of this angle
		 *\~french
		 *\brief		Cosinus trigonométrique
		 *\return		Le cosinus de cet angle
		 */
		CU_API double cos()const;
		/**
		 *\~english
		 *\brief		Trigonometric sine
		 *\return		sine of this angle
		 *\~french
		 *\brief		Sinus trigonométrique
		 *\return		Le sinus de cet angle
		 */
		CU_API double sin()const;
		/**
		 *\~english
		 *\brief		Trigonometric tangent
		 *\return		Tangent of this angle
		 *\~french
		 *\brief		Tangente trigonométrique
		 *\return		La tangente de cet angle
		 */
		CU_API double tan()const;
		/**
		 *\~english
		 *\brief		Hyperbolic cosine
		 *\return		Hyperbolic cosine of this angle
		 *\~french
		 *\brief		Cosinus hyperbolique
		 *\return		Le cosinus hyperbolique de cet angle
		 */
		CU_API double cosh()const;
		/**
		 *\~english
		 *\brief		Hyperbolic sine
		 *\return		Hyperbolic sine of this angle
		 *\~french
		 *\brief		Sinus hyperbolique
		 *\return		Le sinus hyperbolique de cet angle
		 */
		CU_API double sinh()const;
		/**
		 *\~english
		 *\brief		Hyperbolic tangent
		 *\return		Hyperbolic tangent of this angle
		 *\~french
		 *\brief		Tangente hyperbolique
		 *\return		La tangente hyperbolique de cet angle
		 */
		CU_API double tanh()const;
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given cosine value
		 *\param[in]	p_rValue	The cosine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du cosinus donné
		 *\param[in]	p_rValue	Le cosinus
		 */
		CU_API void acos( double p_rValue );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given sine value
		 *\param[in]	p_rValue	The sine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du sinus donné
		 *\param[in]	p_rValue	Le sinus
		 */
		CU_API void asin( double p_rValue );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given tangent value
		 *\param[in]	p_rValue	The tangent value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir de la tangente donné
		 *\param[in]	p_rValue	La tangente
		 */
		CU_API void atan( double p_rValue );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_rhs	Angle to add to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par addition
		 *\param[in]	p_rhs	L'angle à ajouter à celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator+=( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_rhs	Angle to substract to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par soustraction
		 *\param[in]	p_rhs	L'angle à soustraire de celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator-=( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	Angle to multiply to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par multiplication
		 *\param[in]	p_rhs	L'angle à multiplier à celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator*=( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_rhs	Angle divider of this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par division
		 *\param[in]	p_rhs	L'angle diviseur de celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator/=( Angle const & p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_rhs	Scalar to multiply to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par multiplication
		 *\param[in]	p_rhs	Le scalaire à multiplier à celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator*=( double p_rhs );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_rhs	Scalar divider of this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par division
		 *\param[in]	p_rhs	Le scalaire diviseur de celui-ci
		 *\return		Une référence sur cet angle
		 */
		CU_API Angle & operator/=( double p_rhs );

	private:
		//!\~english The angle value	\~french La valeur de l'angle
		double m_radians;
		friend CU_API bool operator==( Angle const & p_lhs, Angle const & p_rhs );
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is equal to p_rhs
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		\p true si p_lhs est égal à p_rhs
	 */
	CU_API bool operator==( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is different from p_rhs
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		\p true si p_lhs est différent de p_rhs
	 */
	CU_API bool operator!=( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of addition
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de l'addition
	 */
	CU_API Angle operator+( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of substraction
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de la soustraction
	 */
	CU_API Angle operator-( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of multiplication
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de la multiplication
	 */
	CU_API Angle operator*( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of division
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de la division
	 */
	CU_API Angle operator/( Angle const & p_lhs, Angle const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of multiplication
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de la multiplication
	 */
	CU_API Angle operator*( Angle const & p_lhs, double p_rhs );
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of division
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de la division
	 */
	CU_API Angle operator/( Angle const & p_lhs, double p_rhs );
}

inline Castor::Angle operator "" _degrees( long double p_value )
{
	return Castor::Angle::from_degrees( p_value );
}

inline Castor::Angle operator "" _radians( long double p_value )
{
	return Castor::Angle::from_radians( p_value );
}

#endif
