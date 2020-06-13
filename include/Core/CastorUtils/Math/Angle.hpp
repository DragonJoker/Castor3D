/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ANGLE_H___
#define ___CASTOR_ANGLE_H___

#include "CastorUtils/Math/MathModule.hpp"

namespace castor
{
	template< typename Type >
	class AngleT
	{
	public:
		//!\~english	Radian to degree conversion constant.
		//!\~french		Constante de conversion de radian vers degré.
		CU_API static constexpr Type RadianToDegree{ Type( 57.295779513082320876798154814105 ) };
		//!\~english	Radian to gradient conversion constant.
		//!\~french		Constante de conversion de radian vers gradient.
		CU_API static constexpr Type RadianToGrad{ 200 / Pi< Type > };
		//!\~english	Degree to radian conversion constant.
		//!\~french		Constante de conversion de degré vers radian.
		CU_API static constexpr Type DegreeToRadian{ Type( 0.01745329251994329576923690768489 ) };
		//!\~english	Degree to gradient conversion constant.
		//!\~french		Constante de conversion de degré vers gradient.
		CU_API static constexpr Type DegreeToGrad{ Type( 200 ) / 180 };
		//!\~english	Gradient to degree conversion constant.
		//!\~french		Constante de conversion de gradient vers degré.
		CU_API static constexpr Type GradToDegree{ Pi< Type > / 200 };
		//!\~english	Gradient to radian conversion constant.
		//!\~french		Constante de conversion de gradient vers radian.
		CU_API static constexpr Type GradToRadian{ 180 / Type( 200 ) };

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
		explicit AngleT( T radians )noexcept
			: m_radians( Type( radians ) )
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Named constructor, from a degrees angle
		 *\param[in]	degrees	The angle in degrees
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en degrés
		 *\param[in]	degrees	L'angle, exprimé en degrés
		 *\return		L'angle construit
		 */
		template< typename T >
		static AngleT< Type > fromDegrees( T degrees )noexcept
		{
			return AngleT< Type >( degrees * DegreeToRadian );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from a radians angle
		 *\param[in]	radians	The angle in radians
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en radians
		 *\param[in]	radians	L'angle, exprimé en radians
		 *\return		L'angle construit
		 */
		template< typename T >
		static AngleT< Type > fromRadians( T radians )noexcept
		{
			return AngleT< Type >( radians );
		}
		/**
		 *\~english
		 *\brief		Named constructor, from a gradients angle
		 *\param[in]	grads	The angle in gradients
		 *\return		The built angle
		 *\~french
		 *\brief		Constructeur nommé, à partir d'un angle en gradients
		 *\param[in]	grads	L'angle, exprimé en gradients
		 *\return		L'angle construit
		 */
		template< typename T >
		static AngleT< Type > fromGrads( T grads )noexcept
		{
			return AngleT< Type >( grads * GradToRadian );
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
		 *\param[in]	value	Angle in degrees
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de degrés
		 *\param[in]	value	L'angle exprimé en degrés
		 */
		inline void degrees( double value )noexcept;
		/**
		 *\~english
		 *\brief		sets this angle value from radians
		 *\param[in]	value	Angle in radians
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de radians
		 *\param[in]	value	L'angle exprimé en radians
		 */
		inline void radians( double value )noexcept;
		/**
		 *\~english
		 *\brief		sets this angle value from gradients
		 *\param[in]	value	Angle in gradients
		 *\~french
		 *\brief		Définit la valeur de cet angle à partir de gradients
		 *\param[in]	value	L'angle exprimé en gradients
		 */
		inline void grads( double value )noexcept;
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
		 *\param[in]	value	The cosine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du cosinus donné
		 *\param[in]	value	Le cosinus
		 */
		inline void acos( double value );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given sine value
		 *\param[in]	value	The sine value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir du sinus donné
		 *\param[in]	value	Le sinus
		 */
		inline void asin( double value );
		/**
		 *\~english
		 *\brief		Computes this angle's value from the given tangent value
		 *\param[in]	value	The tangent value
		 *\~french
		 *\brief		Calcule la valeur de cet angle à partir de la tangente donné
		 *\param[in]	value	La tangente
		 */
		inline void atan( double value );
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
		 *\param[in]	rhs	Angle to add to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par addition
		 *\param[in]	rhs	L'angle à ajouter à celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator+=( AngleT< Type > const & rhs )noexcept;
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	rhs	Angle to subtract to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par soustraction
		 *\param[in]	rhs	L'angle à soustraire de celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator-=( AngleT< Type > const & rhs )noexcept;
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	Angle to multiply to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par multiplication
		 *\param[in]	rhs	L'angle à multiplier à celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator*=( AngleT< Type > const & rhs )noexcept;
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	rhs	Angle divider of this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par division
		 *\param[in]	rhs	L'angle diviseur de celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator/=( AngleT< Type > const & rhs )noexcept;
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	rhs	Scalar to multiply to this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par multiplication
		 *\param[in]	rhs	Le scalaire à multiplier à celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator*=( double rhs )noexcept;
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	rhs	Scalar divider of this one
		 *\return		A reference to this angle
		 *\~french
		 *\brief		Opérateur d'affection par division
		 *\param[in]	rhs	Le scalaire diviseur de celui-ci
		 *\return		Une référence sur cet angle
		 */
		inline AngleT< Type > & operator/=( double rhs )noexcept;
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
		friend bool operator==( AngleT< T > const & lhs, AngleT< T > const & rhs )noexcept;
		template< typename T >
		friend bool operator<( AngleT< T > const & lhs, AngleT< T > const & rhs )noexcept;
		template< typename T >
		friend bool operator>( AngleT< T > const & lhs, AngleT< T > const & rhs )noexcept;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is equal to rhs
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		\p true si lhs est égal à rhs
	 */
	template< typename Type >
	inline bool operator==( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is different from rhs
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		\p true si lhs est différent de rhs
	 */
	template< typename Type >
	inline bool operator!=( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Less than" operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is less than rhs.
	 *\~french
	 *\brief		Opérateur "inférieur à"
	 *\param[in]	lhs	Premier opérande.
	 *\param[in]	rhs	Second opérande.
	 *\return		\p true si lhs est inférieur à rhs.
	 */
	template< typename Type >
	inline bool operator<( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Greater than or equal to" operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is greater than or equal to rhs.
	 *\~french
	 *\brief		Opérateur "supérieur ou égal à"
	 *\param[in]	lhs	Premier opérande.
	 *\param[in]	rhs	Second opérande.
	 *\return		\p true si lhs est supérieur ou égal à rhs.
	 */
	template< typename Type >
	inline bool operator>=( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Greater than" operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is greater than rhs.
	 *\~french
	 *\brief		Opérateur "supérieur à"
	 *\param[in]	lhs	Premier opérande.
	 *\param[in]	rhs	Second opérande.
	 *\return		\p true si lhs est supérieur à rhs.
	 */
	template< typename Type >
	inline bool operator>( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		"Less than or equal to" operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		\p true if lhs is less than or equal to rhs.
	 *\~french
	 *\brief		Opérateur "inférieur ou égal à"
	 *\param[in]	lhs	Premier opérande.
	 *\param[in]	rhs	Second opérande.
	 *\return		\p true si lhs est inférieur ou égal à rhs.
	 */
	template< typename Type >
	inline bool operator<=( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		addition operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of addition
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de l'addition
	 */
	template< typename Type >
	inline AngleT< Type > operator+( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of substraction
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de la soustraction
	 */
	template< typename Type >
	inline AngleT< Type > operator-( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of multiplication
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de la multiplication
	 */
	template< typename Type >
	inline AngleT< Type > operator*( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of division
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de la division
	 */
	template< typename Type >
	inline AngleT< Type > operator/( AngleT< Type > const & lhs, AngleT< Type > const & rhs )noexcept;
	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of multiplication
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de la multiplication
	 */
	template< typename Type >
	inline AngleT< Type > operator*( AngleT< Type > const & lhs, double rhs )noexcept;
	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	lhs	First operand
	 *\param[in]	rhs	Second operand
	 *\return		Result of division
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	lhs	Premier opérande
	 *\param[in]	rhs	Second opérande
	 *\return		Le résultat de la division
	 */
	template< typename Type >
	inline AngleT< Type > operator/( AngleT< Type > const & lhs, double rhs )noexcept;

	using Angle = AngleT< float >;
}

inline castor::Angle operator "" _degrees( long double value )
{
	return castor::Angle::fromDegrees( value );
}

inline castor::Angle operator "" _radians( long double value )
{
	return castor::Angle::fromRadians( value );
}

#include "Angle.inl"

#endif
