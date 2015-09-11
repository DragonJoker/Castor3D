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
#ifndef ___CASTOR_TEMPLATES_H___
#define ___CASTOR_TEMPLATES_H___

#include <limits>
#include <algorithm>
#include <functional>
#include "StringUtils.hpp"
#include "Math.hpp"

namespace Castor
{
	namespace details
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Functor used to delete an object with for_each
		\~french
		\brief		Foncteur utilisé pour désallouer un objet dans un for_each
		*/
		template< typename ObjType >
		struct obj_deleter
		{
			void operator()( ObjType * p_pPointer )
			{
				delete p_pPointer;
			}
		};
		/**
		 *\~english
		 *\brief		Deletes all container's objects
		 *\param[in]	p_container	The container
		 *\param[in]	p_object	A dummy object
		 *\~french
		 *\brief		Désalloue tous les objets du conteneur
		 *\param[in]	p_container	Le conteneur
		 *\param[in]	p_object	Un objet bidon
		 */
		template< typename CtnrType, typename ObjType >
		void clear_content( CtnrType & CU_PARAM_UNUSED( p_container ), ObjType CU_PARAM_UNUSED( p_object ) )
		{
		}
		/**
		 *\~english
		 *\brief		Deletes all container's objects
		 *\param[in]	p_container	The container
		 *\param[in]	p_object	A dummy object
		 *\~french
		 *\brief		Désalloue tous les objets du conteneur
		 *\param[in]	p_container	Le conteneur
		 *\param[in]	p_object	Un objet bidon
		 */
		template< typename CtnrType, typename ObjType >
		void clear_content( CtnrType & p_container, ObjType * CU_PARAM_UNUSED( p_object ) )
		{
			std::for_each( p_container.begin(), p_container.end(), obj_deleter< ObjType >() );
		}
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Functor used to delete a pair with for_each
		\~french
		\brief		Foncteur utilisé pour désallouer une paire dans un for_each
		*/
		template< typename KeyType, typename ObjType >
		struct pair_deleter
		{
			void operator()( std::pair< KeyType, ObjType * > p_pair )
			{
				delete p_pair.second;
			}
		};
		/**
		 *\~english
		 *\brief		Deletes all container's pairs
		 *\param[in]	p_container	The container
		 *\param[in]	p_object	A dummy pair
		 *\~french
		 *\brief		Désalloue toutes les paires du conteneur
		 *\param[in]	p_container	Le conteneur
		 *\param[in]	p_object	Une paire bidon
		 */
		template< class CtnrType, typename KeyType, typename ObjType >
		void clear_pair_content( CtnrType & CU_PARAM_UNUSED( p_container ), std::pair< KeyType, ObjType > CU_PARAM_UNUSED( p_pair ) )
		{
		}
		/**
		 *\~english
		 *\brief		Deletes all container's pairs
		 *\param[in]	p_container	The container
		 *\param[in]	p_object	A dummy pair
		 *\~french
		 *\brief		Désalloue toutes les paires du conteneur
		 *\param[in]	p_container	Le conteneur
		 *\param[in]	p_object	Une paire bidon
		 */
		template< class CtnrType, typename KeyType, typename ObjType >
		void clear_pair_content( CtnrType & p_container, std::pair< KeyType, ObjType * > CU_PARAM_UNUSED( p_pair ) )
		{
			std::for_each( p_container.begin(), p_container.end(), pair_deleter< KeyType, ObjType >() );
		}
	}
	/**
	 *\~english
	 *\brief		Clears a container
	 *\remark		Deallocates all the content if needed
	 *\param[in]	p_container	The container to clear
	 *\~french
	 *\brief		Vide un conteneur
	 *\remark		Désalloue le contenu si besoin est
	 *\param[in]	p_container	Le contenur à vider
	 */
	template< class CtnrType >
	void clear_container( CtnrType & p_container )
	{
		typedef typename CtnrType::value_type value_type;
		details::clear_content( p_container, value_type() );
		CtnrType().swap( p_container );
	}
	/**
	 *\~english
	 *\brief		Clears a pair container (like std::map)
	 *\remark		Deallocates all the content if needed
	 *\param[in]	p_container	The container to clear
	 *\~french
	 *\brief		Vide un conteneur de paires (std::map, par exemple)
	 *\remark		Désalloue le contenu si besoin est
	 *\param[in]	p_container	Le contenur à vider
	 */
	template< class CtnrType >
	void clear_pair_container( CtnrType & p_container )
	{
		typedef typename CtnrType::value_type value_type;
		details::clear_pair_content( p_container, value_type() );
		CtnrType().swap( p_container );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Struct used to select best way to put type in parameter : 'value' or 'const reference'
	\~french
	\brief		Structure utilisée pour récupéerer la meilleure façon de passer T en paramètre : valeur ou référence constante
	*/
	template< typename T >
	struct call_traits
	{
	private:
		template< typename U, bool Big > struct call_traits_helper;
		template< typename U >
		struct call_traits_helper< U, true >
		{
			typedef U const & const_param_type;
			typedef U & param_type;
		};
		template< typename U >
		struct call_traits_helper< U, false >
		{
			typedef U const const_param_type;
			typedef U param_type;
		};

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Typedef over the best way to put type in parameter : 'value' or 'const reference'
		\~french
		\brief		Typedef sur la meilleure façon de passer T en paramètre : valeur ou référence constante
		*/
		typedef typename call_traits_helper < T, ( sizeof( T ) > sizeof( void * ) ) >::const_param_type const_param_type;
		typedef typename call_traits_helper < T, ( sizeof( T ) > sizeof( void * ) ) >::param_type param_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Policy for various types
	\remark		Policy created to use various types in template classes such as Point, Matrix, and others.
				<br />Holds  operators, initialisation functions, conversion functions, ...
	\~french
	\brief		Politique pour différents types
	\remark		Police créée pour utiliser différents types dans les classes template Point, Matrix, ...
				<br />Contient des opérateurs, fonctions d'initialisation, de conversion, ...
	*/
	template< typename T > class Policy
	{
	private:
		typedef T value_type;
		typedef typename call_traits< value_type >::const_param_type param_type;

	public:
		/**
		 *\~english
		 *\return		The neutral value for the type
		 *\~french
		 *\return		La valeur neutre pour le type
		 */
		static value_type zero()
		{
			return value_type();
		}
		/**
		 *\~english
		 *\return		The unit value for the type
		 *\~french
		 *\return		La valeur unitaire pour le type
		 */
		static value_type unit()
		{
			return 1;
		}
		/**
		 *\~english
		 *\brief		Initialises the given variable to the neutral value
		 *\param[in]	p_a	The variable to initialise
		 *\~french
		 *\brief		Initialise La variable donnée à la valeur neutre
		 *\param[in]	p_a	La variable à initialiser
		 */
		static void init( value_type & p_a )
		{
			p_a = zero();
		}
		/**
		 *\~english
		 *\brief		Tests if the given param is equal to neutral value
		 *\param[in]	p_a	The value to test
		 *\return		The test result
		 *\~french
		 *\brief		Teste si le param donné est égal à la valeur neutre
		 *\param[in]	p_a	La valeur à tester
		 *\return		Le résultat du test
		 */
		static bool is_null( param_type p_a )
		{
			return equals<T>( p_a, zero() );
		}
		/**
		 *\~english
		 *\param[in]	p_a	The value to negate
		 *\return		The negated param (opposite of the value, relative to the neutral value)
		 *\~french
		 *\param[in]	p_a	La valeur à négativer
		 *\return		La valeur négativée du param (opposé de la valeur, relativement à la valeur neutre)
		 */
		static value_type negate( param_type p_a )
		{
			return std::is_unsigned<value_type>::value ? p_a : -p_a;
		}
		/**
		 *\~english
		 *\param[in,out]	p_a	The value to negate, received the negated value
		 *\return			Reference to the param
		 *\~french
		 *\param[in,out]	p_a	La valeur à négativer, reçoit la valeur négativée
		 *\return			Référence sur le paramètre
		 */
		static value_type & ass_negate( value_type & p_a )
		{
			return assign<T>( p_a, negate( p_a ) );
		}
		/**
		 *\~english
		 *\brief		Extracts a value of the type from a String
		 *\param[in]	p_str	The string containing the value to extract
		 *\return		The retrieved value, neutral if invalid
		 *\~french
		 *\brief		Extrait une valeur du type donné à partir d'une chaîne de caractères
		 *\param[in]	p_str	La chaîne de caractères contenant la valeur à extraire
		 *\return		La valeur récupérée, neutre si invalide
		 */
		static value_type parse( String const & p_str )
		{
			value_type l_tReturn;
			str_utils::parse( p_str, l_tReturn );
			return l_tReturn;
		}
		/**
		 *\~english
		 *\brief		If the type is a floating type, rounds it, else doesn't do anything
		 *\param[in]	p_a	The value to stick
		 *\~french
		 *\brief		Si le type est un type flottant, arrondit le paramètre, sinon ne fait rien
		 *\param[in]	p_a	La valeur à arrondir
		 */
		static void stick( value_type & p_a )
		{
			if ( std::is_floating_point< value_type >::value )
			{
				if ( is_null( p_a ) )
				{
					init( p_a );
				}
				else if ( p_a > 0 )
				{
					if ( equals( p_a, double( int( p_a + 0.5 ) ) ) )
					{
						assign( p_a, double( int( p_a + 0.5 ) ) );
					}
				}
				else
				{
					if ( equals( p_a, double( int( p_a - 0.5 ) ) ) )
					{
						assign( p_a, double( int( p_a - 0.5 ) ) );
					}
				}
			}
		}
		/**
		 *\~english
		 *\brief		Converts a given param of a given type to this class template type
		 *\param[in]	p_value	The value to convert
		 *\return		The converted value (static_cast, essentially)
		 *\~french
		 *\brief		Convertit un param d'un type donné vers le type template de cette classe
		 *\param[in]	p_value	La valeur à convertir
		 *\return		La valeur convertie (static_cast)
		 */
		template< typename Ty > static value_type convert( Ty const & p_value )
		{
			return static_cast< value_type >( p_value );
		}
		/**
		 *\~english
		 *\brief		Tests equality between 2 params of different types. uses std::numeric_limits::epsilon to perform the test
		 *\param[in]	p_a	The first param, whose type is used as the template argument of std::numeric_limits
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before comparing
		 *\return		The compare result
		 *\~french
		 *\brief		Teste l'égalité entre 2 params de type différent, utilise  std::numeric_limits::epsilon pour faire le test
		 *\param[in]	p_a	Le premier param, dont le type est pris comme argument template de std::numeric_limits
		 *\param[in]	p_b	Le second param, converti dans le type de \ p_a avant la comparaison
		 *\return		Le résultat de la comparaison
		 */
		template< typename Ty > static bool equals( param_type p_a, Ty const & p_b )
		{
			return abs( p_a - convert< Ty >( p_b ) ) <= std::numeric_limits< value_type >::epsilon();
		}
		/**
		 *\~english
		 *\brief		Adds two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before adding
		 *\return		The addition result
		 *\~french
		 *\brief		Additionne 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant l'addition
		 *\return		Le résultat de l'addition
		 */
		template< typename Ty > static value_type add( param_type p_a, Ty const & p_b )
		{
			return p_a + convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Substracts two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before substracting
		 *\return		The substraction result
		 *\~french
		 *\brief		Soustrait 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la soustraction
		 *\return		Le résultat de la soustraction
		 */
		template< typename Ty > static value_type substract( param_type p_a, Ty const & p_b )
		{
			return p_a - convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Multiplies two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return		The multiplication result
		 *\~french
		 *\brief		Multiplie 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la multiplication
		 *\return		Le résultat de la multiplication
		 */
		template< typename Ty > static value_type multiply( param_type p_a, Ty const & p_b )
		{
			return p_a * convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Divides two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before dividing
		 *\return	The division result
		 *\~french
		 *\brief		Divise 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la division
		 *\return		Le résultat de la division
		 */
		template< typename Ty > static value_type divide( param_type p_a, Ty const & p_b )
		{
			return p_a / convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief			Adds two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before adding
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Additionne 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant l'addition
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_add( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, add< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Substracts two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before substracting
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Soustrait 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la soustraction
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_substract( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, substract< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Multiplies two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Multiplie 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la multiplication
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_multiply( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, multiply< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Divides two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before dividing
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Divise 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la division
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_divide( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, divide< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Converts the second param and assigns the result to the first param
		 *\param[in,out]	p_a	The first param, receives the converted second param
		 *\param[in]		p_b	The second param
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Convertit le second param et affecte le résultat au premier
		 *\param[in,out]	p_a	Le premier param, reçoit le second param converti
		 *\param[in]		p_b	Le second param
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & assign( value_type & p_a, Ty const & p_b )
		{
			return p_a = convert< Ty >( p_b );
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		'bool' Policy class specialisation
	\~french
	\brief		Spécialisation de la classe Policy pour les 'bool'
	*/
	template <> class Policy< bool >
	{
	private:
		typedef bool value_type;
		typedef call_traits<value_type>::const_param_type param_type;

	public:
		/**
		 *\~english
		 *\return		The neutral value for the type
		 *\~french
		 *\return		La valeur neutre pour le type
		 */
		static value_type zero()
		{
			return false;
		}
		/**
		 *\~english
		 *\return		The unit value for the type
		 *\~french
		 *\return		La valeur unitaire pour le type
		 */
		static value_type unit()
		{
			return true;
		}
		/**
		 *\~english
		 *\brief		Initialises the given variable to the neutral value
		 *\param[in]	p_a	The variable to initialise
		 *\~french
		 *\brief		Initialise La variable donnée à la valeur neutre
		 *\param[in]	p_a	La variable à initialiser
		 */
		static void init( value_type & p_a )
		{
			p_a = zero();
		}
		/**
		 *\~english
		 *\brief		Tests if the given param is equal to neutral value
		 *\param[in]	p_a	The value to test
		 *\return		The test result
		 *\~french
		 *\brief		Teste si le param donné est égal à la valeur neutre
		 *\param[in]	p_a	La valeur à tester
		 *\return		Le résultat du test
		 */
		static bool is_null( value_type p_a )
		{
			return !p_a;
		}
		/**
		 *\~english
		 *\param[in]	p_a	The value to negate
		 *\return		The negated param (opposite of the value, relative to the neutral value)
		 *\~french
		 *\param[in]	p_a	La valeur à négativer
		 *\return		La valeur négativée du param (opposé de la valeur, relativement à la valeur neutre)
		 */
		static value_type negate( value_type p_a )
		{
			return ! p_a;
		}
		/**
		 *\~english
		 *\param[in,out]	p_a	The value to negate, received the negated value
		 *\return			Reference to the param
		 *\~french
		 *\param[in,out]	p_a	La valeur à négativer, reçoit la valeur négativée
		 *\return			Référence sur le paramètre
		 */
		static value_type & ass_negate( value_type & p_a )
		{
			return assign< value_type >( p_a, negate( p_a ) );
		}
		/**
		 *\~english
		 *\brief		Extracts a value of the type from a String
		 *\param[in]	p_strVal	The string containing the value to extract
		 *\return		The retrieved value, neutral if invalid
		 *\~french
		 *\brief		Extrait une valeur du type donné à partir d'une chaîne de caractères
		 *\param[in]	p_strVal	La chaîne de caractères contenant la valeur à extraire
		 *\return		La valeur récupérée, neutre si invalide
		 */
		static value_type parse( String const & p_strVal )
		{
			return p_strVal == cuT( "true" ) || p_strVal == cuT( "TRUE" ) || p_strVal == cuT( "1" ) || str_utils::to_int( p_strVal ) != 0;
		}
		/**
		 *\~english
		 *\brief		If the type is a floating type, rounds it, else doesn't do anything
		 *\param[in]	p_a	The value to stick
		 *\~french
		 *\brief		Si le type est un type flottant, arrondit le paramètre, sinon ne fait rien
		 *\param[in]	p_a	La valeur à arrondir
		 */
		static void stick( value_type & ) {}
		/**
		 *\~english
		 *\brief		Converts a given param of a given type to this class template type
		 *\param[in]	p_value	The value to convert
		 *\return		The converted value (static_cast, essentially)
		 *\~french
		 *\brief		Convertit un param d'un type donné vers le type template de cette classe
		 *\param[in]	p_value	La valeur à convertir
		 *\return		La valeur convertie (static_cast)
		 */
		template< typename Ty > static value_type convert( Ty const & p_value )
		{
			return ! Policy< Ty >::is_null( p_value );
		}
		/**
		 *\~english
		 *\brief		Tests equality between 2 params of different types. uses std::numeric_limits::epsilon to perform the test
		 *\param[in]	p_a	The first param, whose type is used as the template argument of std::numeric_limits
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before comparing
		 *\return		The compare result
		 *\~french
		 *\brief		Teste l'égalité entre 2 params de type différent, utilise  std::numeric_limits::epsilon pour faire le test
		 *\param[in]	p_a	Le premier param, dont le type est pris comme argument template de std::numeric_limits
		 *\param[in]	p_b	Le second param, converti dans le type de \ p_a avant la comparaison
		 *\return		Le résultat de la comparaison
		 */
		template< typename Ty > static bool equals( param_type p_a, Ty const & p_b )
		{
			return p_a == convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Adds two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before adding
		 *\return		The addition result
		 *\~french
		 *\brief		Additionne 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant l'addition
		 *\return		Le résultat de l'addition
		 */
		template< typename Ty > static value_type add( param_type p_a, Ty const & p_b )
		{
			return p_a || convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Substracts two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before substracting
		 *\return		The substraction result
		 *\~french
		 *\brief		Soustrait 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la soustraction
		 *\return		Le résultat de la soustraction
		 */
		template< typename Ty > static value_type substract( param_type p_a, Ty const & p_b )
		{
			return p_a || convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Multiplies two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return		The multiplication result
		 *\~french
		 *\brief		Multiplie 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la multiplication
		 *\return		Le résultat de la multiplication
		 */
		template< typename Ty > static value_type multiply( param_type p_a, Ty const & p_b )
		{
			return p_a && convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief		Divides two params of different type, gives the result
		 *\param[in]	p_a	The first param
		 *\param[in]	p_b	The second param, converted into \p p_a 's type before dividing
		 *\return		The division result
		 *\~french
		 *\brief		Divise 2 param de type différent, retourne le résultat
		 *\param[in]	p_a	Le premier param
		 *\param[in]	p_b	Le second param, converti dans le type de \p p_a avant la division
		 *\return		Le résultat de la division
		 */
		template< typename Ty > static value_type divide( param_type p_a, Ty const & p_b )
		{
			return p_a && convert< Ty >( p_b );
		}
		/**
		 *\~english
		 *\brief			Adds two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before adding
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Additionne 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant l'addition
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_add( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, add< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Substracts two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before substracting
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Soustrait 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la soustraction
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_substract( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, substract< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Multiplies two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before multiplying
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Multiplie 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la multiplication
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_multiply( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, multiply< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Divides two params of different type, gives the result
		 *\remark			The first param receives the result
		 *\param[in,out]	p_a	The first param, receives the result
		 *\param[in]		p_b	The second param, converted into \p p_a 's type before dividing
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Divise 2 param de type différent, retourne le résultat
		 *\remark			Le premier param reçoit le résultat
		 *\param[in,out]	p_a	Le premier param, reçoit le résultat
		 *\param[in]		p_b	Le second param, converti dans le type de \p p_a avant la division
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & ass_divide( value_type & p_a, Ty const & p_b )
		{
			return assign( p_a, divide< Ty >( p_a, p_b ) );
		}
		/**
		 *\~english
		 *\brief			Converts the second param and assigns the result to the first param
		 *\param[in,out]	p_a	The first param, receives the converted second param
		 *\param[in]		p_b	The second param
		 *\return			Reference to the first param
		 *\~french
		 *\brief			Convertit le second param et affecte le résultat au premier
		 *\param[in,out]	p_a	Le premier param, reçoit le second param converti
		 *\param[in]		p_b	Le second param
		 *\return			La référence sur le premier param
		 */
		template< typename Ty > static value_type & assign( value_type & p_a, Ty const & p_b )
		{
			return p_a = convert< Ty >( p_b );
		}
	};
}

#endif
