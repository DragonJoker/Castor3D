/*
See LICENSE file in root folder
*/
#ifndef ___CU_SIMD_H___
#define ___CU_SIMD_H___

#include "CastorUtilsPrerequisites.hpp"

#if CU_UseSSE2

#include <xmmintrin.h>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		06/10/2016
	\~english
	\brief		SIMD 4 floats abstraction.
	\~french
	\brief		Abstraction de 4 floats SIMD.
	*/
	class Float4
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor from a pointer.
		 *\param[in]	p_values	A 16 bits aligned pointer to 4 floats.
		 *\~french
		 *\brief		Constructeur depuis un pointeur.
		 *\param[in]	p_values	Un pointeur sur 4 flottants alignés sur 16 bits.
		 */
		explicit inline Float4( float const * p_values );
		/**
		 *\~english
		 *\brief		Constructor from a single value (duplicates it).
		 *\param[in]	p_value	The value.
		 *\~french
		 *\brief		Constructeur depuis une simple valeur (elle sera dupliquée).
		 *\param[in]	p_value	La valeur.
		 */
		explicit inline Float4( float p_value );
		/**
		 *\~english
		 *\brief		Puts the values into a pointer.
		 *\param[out]	p_values	A 16 bits aligned pointer to 4 floats.
		 *\~french
		 *\brief		Met les valeurs dans un pointeur.
		 *\param[out]	p_values	Un pointeur sur 4 flottants alignés sur 16 bits.
		 */
		inline void toPtr( float * p_values );
		/**
		 *\~english
		 *\brief		addition assignment operator.
		 *\param[in]	p_rhs	The right hand side operand.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par addition.
		 *\param[in]	p_rhs	L'opérande de droite dans l'opération.
		 *\return		Une référence sur cet objet.
		 */
		inline Float4 & operator+=( Float4 const & p_rhs );
		/**
		 *\~english
		 *\brief		Subtraction assignment operator.
		 *\param[in]	p_rhs	The right hand side operand.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction.
		 *\param[in]	p_rhs	L'opérande de droite dans l'opération.
		 *\return		Une référence sur cet objet.
		 */
		inline Float4 & operator-=( Float4 const & p_rhs );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator.
		 *\param[in]	p_rhs	The right hand side operand.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication.
		 *\param[in]	p_rhs	L'opérande de droite dans l'opération.
		 *\return		Une référence sur cet objet.
		 */
		inline Float4 & operator*=( Float4 const & p_rhs );
		/**
		 *\~english
		 *\brief		Division assignment operator.
		 *\param[in]	p_rhs	The right hand side operand.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par division.
		 *\param[in]	p_rhs	L'opérande de droite dans l'opération.
		 *\return		Une référence sur cet objet.
		 */
		inline Float4 & operator/=( Float4 const & p_rhs );

	private:
		__m128 m_value;
	};
	/**
	 *\~english
	 *\brief		addition operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The addition result.
	 *\~french
	 *\brief		Opérateur d'addition.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		Le résultat de l'addition.
	 */
	inline Float4 operator+( Float4 const & p_lhs, Float4 const & p_rhs );
	/**
	 *\~english
	 *\brief		Subtraction operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The subtraction result.
	 *\~french
	 *\brief		Opérateur de soustraction.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		Le résultat de la soustraction.
	 */
	inline Float4 operator-( Float4 const & p_lhs, Float4 const & p_rhs );
	/**
	 *\~english
	 *\brief		Multiplication operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The multiplication result.
	 *\~french
	 *\brief		Opérateur de multiplication.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		Le résultat de la multiplication.
	 */
	inline Float4 operator*( Float4 const & p_lhs, Float4 const & p_rhs );
	/**
	 *\~english
	 *\brief		Division operator.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\return		The division result.
	 *\~french
	 *\brief		Opérateur de division.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 *\return		Le résultat de la division.
	 */
	inline Float4 operator/( Float4 const & p_lhs, Float4 const & p_rhs );
}

#include "Simd.inl"

#endif
#endif

