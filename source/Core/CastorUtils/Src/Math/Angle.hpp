/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ANGLE_H___
#define ___CASTOR_ANGLE_H___

#include "CastorUtils.hpp"

namespace castor
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
	template< typename Type >
	class AngleT
	{
	public:
		//!\~english Radian to degree conversion constant	\~french Constante de conversion de radian vers degré
		CU_API static const Type RadianToDegree;
		//!\~english Radian to gradient conversion constant	\~french Constante de conversion de radian vers gradient
		CU_API static const Type RadianToGrad;
		//!\~english Degree to radian conversion constant	\~french Constante de conversion de degré vers radian
		CU_API static const Type DegreeToRadian;
		//!\~english Degree to gradient conversion constant	\~french Constante de conversion de degré vers gradient
		CU_API static const Type DegreeToGrad;
		//!\~english Gradient to degree conversion constant	\~french Constante de conversion de gradient vers degré
		CU_API static const Type GradToDegree;
		//!\~english Gradient to radian conversion constant	\~french Constante de conversion de gradient vers radian
		CU_API static const Type GradToRadian;
		//!\~english Pi constant, expressed in real	\~french Constante pi exprimée en real
		CU_API static const Type Pi;
		//!\~english Pi * 2 constant	\~french Constante pi * 2
		CU_API static const Type PiMult2;
		//!\~english Pi / 2 constant	\~french Constante pi / 2
		CU_API static const Type PiDiv2;

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
		explicit AngleT( T p_radians )noexcept
			: m_radians( Type( p_radians ) )
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
		static AngleT< Type > fromDegrees( T p_degrees )noexcept
		{
			return AngleT< Type >( p_degrees * DegreeToRadian );
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
		static AngleT< Type > fromRadians( T p_radians )noexcept
		{
			return AngleT< Type >( p_radians );
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
		static AngleT< Type > fromGrads( T p_grads )noexcept
		{
			return AngleT< Type >( p_grads * GradToRadian );
		}
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		inline AngleT()noexcept;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~AngleT()noexcept;
		/**
		 *\~english
		 *\brief		Conversion to degrees
		 *\return		Angle value, in degrees
		 *\~french
		 *\brief		Conversion en degrés
		 *\return		La valeur de l'angle, en degrés
		 */
		inline Type degrees()const noexcept;
		/**
		 *\~english
		 *\brief		Conversion to radians
		 *\return		Angle value, in radians
		 *\~french
		 *\brief		Conversion en radians
		 *\return		La valeur de l'angle, en radians
		 */
		inline Type radians()const noexcept;
		/**
		 *\~english
		 *\brief		Conversion to gradients
		 *\return		Angle value, in gradients
		 *\~french
		 *\brief		Conversion en gradients
		 *\return		La valeur de l'angle, en gradients
		 */
		inline Type grads()const noexcept;
		/**
		 *\~english
		 *\brief		sets this angle value from degrees
		 *\param[in]	p_rAngle	Angle in degrees
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de degrés
		 *\param[in]	p_rAngle	L'angle exprimé en degrés
		 */
		inline void degrees( double p_rAngle )noexcept;
		/**
		 *\~english
		 *\brief		sets this angle value from radians
		 *\param[in]	p_rAngle	Angle in radians
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de radians
		 *\param[in]	p_rAngle	L'angle exprimé en radians
		 */
		inline void radians( double p_rAngle )noexcept;
		/**
		 *\~english
		 *\brief		sets this angle value from gradients
		 *\param[in]	p_rAngle	Angle in gradients
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de gradients
		 *\param[in]	p_rAngle	L'angle exprimé en gradients
		 */
		inline void grads( double p_rAngle )noexcept;
		/**
		 *\~english
		 *\brief		Trigonometric cosine
		 *\return		Cosine of this angle
		 *\~french
		 *\brief		Cosinus trigonométrique
		 *\return		Le cosinus de cet angle
		 */
		inline Type cos()const;
		/**
		 *\~english
		 *\brief		Trigonometric sine
		 *\return		sine of this angle
		 *\~french
		 *\brief		Sinus trigonométrique
		 *\return		Le sinus de cet angle
		 */
		inline Type sin()const;
		/**
		 *\~english
		 *\brief		Trigonometric tangent
		 *\return		Tangent of this angle
		 *\~french
		 *\brief		Tangente trigonométrique
		 *\return		La tangente de cet angle
		 */
		inline Type tan()const;
		/**
		 *\~english
		 *\brief		Hyperbolic cosine
		 *\return		Hyperbolic cosine of this angle
		 *\~french
		 *\brief		Cosinus hyperbolique
		 *\return		Le cosinus hyperbolique de cet angle
		 */
		inline Type cosh()const;
		/**
		 *\~english
		 *\brief		Hyperbolic sine
		 *\return		Hyperbolic sine of this angle
		 *\~french
		 *\brief		Sinus hyperbolique
		 *\return		Le sinus hyperbolique de cet angle
		 */
		inline Type sinh()const;
		/**
		 *\~english
		 *\brief		Hyperbolic tangent
		 *\return		Hyperbolic tangent of this angle
		 *\~french
		 *\brief		Tangente hyperbolique
		 *\return		La tangente hyperbolique de cet angle
		 */
		inline Type tanh()const;
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given cosine value
		 *\param[in]	p_rValue	The cosine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du cosinus donné
		 *\param[in]	p_rValue	Le cosinus
		 */
		inline void acos( double p_rValue );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given sine value
		 *\param[in]	p_rValue	The sine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du sinus donné
		 *\param[in]	p_rValue	Le sinus
		 */
		inline void asin( double p_rValue );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given tangent value
		 *\param[in]	p_rValue	The tangent value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir de la tangente donné
		 *\param[in]	p_rValue	La tangente
		 */
		inline void atan( double p_rValue );
		/**
		 *\~english
		 *\brief		Negation operator.
		 *\return		A reference to this angle.
		 *\~french
		 *\brief		Opérateur de négation.
		 *\return		Une référence sur cet angle.
		 */
		inline AngleT< Type > & operator-()noexcept;
		/**
		 *\~english
		 *\brief		addition assignment operator
		 *\param[in]	p_rhs	Angle to add to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par addition
		 *\param[in]	p_rhs	L'angle à ajouter à celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator+=( AngleT< Type > const & p_rhs )noexcept;
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_rhs	Angle to subtract to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par soustraction
		 *\param[in]	p_rhs	L'angle à soustraire de celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator-=( AngleT< Type > const & p_rhs )noexcept;
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
		inline AngleT< Type > & operator*=( AngleT< Type > const & p_rhs )noexcept;
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
		inline AngleT< Type > & operator/=( AngleT< Type > const & p_rhs )noexcept;
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
		inline AngleT< Type > & operator*=( double p_rhs )noexcept;
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
		inline AngleT< Type > & operator/=( double p_rhs )noexcept;
		/**
		 *\~english
		*\return		The angle value, in radians.
		 *\~french
		*\return		La valeur de l'angle, en radians.
		*/
		explicit inline operator Type()const noexcept
		{
			return m_radians;
		}

	private:
		//!\~english The angle value	\~french La valeur de l'angle
		Type m_radians;

		template< typename T >
		friend bool operator==( AngleT< T > const & p_lhs, AngleT< T > const & p_rhs )noexcept;
		template< typename T >
		friend bool operator<( AngleT< T > const & p_lhs, AngleT< T > const & p_rhs )noexcept;
		template< typename T >
		friend bool operator>( AngleT< T > const & p_lhs, AngleT< T > const & p_rhs )noexcept;
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
	template< typename Type >
	inline bool operator==( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
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
	template< typename Type >
	inline bool operator!=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Less than" operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is less than p_rhs.
	 *\~french
	 *\brief		Opérateur "inférieur à"
	 *\param[in]	p_lhs	Premier opérande.
	 *\param[in]	p_rhs	Second opérande.
	 *\return		\p true si p_lhs est inférieur à p_rhs.
	 */
	template< typename Type >
	inline bool operator<( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Greater than or equal to" operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is greater than or equal to p_rhs.
	 *\~french
	 *\brief		Opérateur "supérieur ou égal à"
	 *\param[in]	p_lhs	Premier opérande.
	 *\param[in]	p_rhs	Second opérande.
	 *\return		\p true si p_lhs est supérieur ou égal à p_rhs.
	 */
	template< typename Type >
	inline bool operator>=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Greater than" operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is greater than p_rhs.
	 *\~french
	 *\brief		Opérateur "supérieur à"
	 *\param[in]	p_lhs	Premier opérande.
	 *\param[in]	p_rhs	Second opérande.
	 *\return		\p true si p_lhs est supérieur à p_rhs.
	 */
	template< typename Type >
	inline bool operator>( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Less than or equal to" operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		\p true if p_lhs is less than or equal to p_rhs.
	 *\~french
	 *\brief		Opérateur "inférieur ou égal à"
	 *\param[in]	p_lhs	Premier opérande.
	 *\param[in]	p_rhs	Second opérande.
	 *\return		\p true si p_lhs est inférieur ou égal à p_rhs.
	 */
	template< typename Type >
	inline bool operator<=( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	p_lhs	First operand
	 *\param[in]	p_rhs	Second operand
	 *\return		Result of addition
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs	Premier opérande
	 *\param[in]	p_rhs	Second opérande
	 *\return		Le résultat de l'addition
	 */
	template< typename Type >
	inline AngleT< Type > operator+( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
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
	template< typename Type >
	inline AngleT< Type > operator-( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
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
	template< typename Type >
	inline AngleT< Type > operator*( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
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
	template< typename Type >
	inline AngleT< Type > operator/( AngleT< Type > const & p_lhs, AngleT< Type > const & p_rhs )noexcept;
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
	template< typename Type >
	inline AngleT< Type > operator*( AngleT< Type > const & p_lhs, double p_rhs )noexcept;
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
	template< typename Type >
	inline AngleT< Type > operator/( AngleT< Type > const & p_lhs, double p_rhs )noexcept;

	using Angle = AngleT< float >;
}

inline castor::Angle operator "" _degrees( long double p_value )
{
	return castor::Angle::fromDegrees( p_value );
}

inline castor::Angle operator "" _radians( long double p_value )
{
	return castor::Angle::fromRadians( p_value );
}

#include "Angle.inl"

#endif
