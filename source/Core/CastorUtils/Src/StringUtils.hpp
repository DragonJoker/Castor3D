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
#ifndef ___CASTOR_STRING_H___
#define ___CASTOR_STRING_H___

#include "CastorUtilsPrerequisites.hpp"
#include "StreamBaseManipulators.hpp"
#include <sstream>

namespace Castor
{
	/*!
	\author Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/06/2012
	\~english
	\brief 		String functions namespace
	\~french
	\brief 		Espace de nom regroupant des fonctions sur les chaînes de caractères
	*/
	namespace string
	{
		/**
		 *\~english
		 *\brief		Tests if the given String represents an integer
		 *\param[in]	p_strToTest	The String to test
		 *\param[in]	p_locale	The locale (unused)
		 *\return		\p false if p_strToTest contains any character other than '0123456789'
		 *\~french
		 *\brief		Teste si le String donné réprésente un nombre entier
		 *\param[in]	p_strToTest Le String à tester
		 *\param[in]	p_locale	La locale (inutilisée)
		 *\return		\p false si p_strToTest contient n'importe quel caractère autre que '0123456789'
		 */
		CU_API bool is_integer( String const & p_strToTest, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Tests if the given String represents a floating number
		 *\param[in]	p_strToTest	The String to test
		 *\param[in]	p_locale	The locale used to determine the dot format
		 *\return		\p false if p_strToTest contains any character other than '0123456789.,' or more than one '.'/','
		 *\~french
		 *\brief		Teste si le String donné réprésente un nombre flottant
		 *\param[in]	p_strToTest Le String à tester
		 *\param[in]	p_locale	La locale utilisée pour déterminer le format du séparateur '.'
		 *\return		\p false si p_strToTest contient n'importe quel caractère autre que '0123456789.,' ou plus d'un '.'/','
		 */
		CU_API bool is_floating( String const & p_strToTest, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a short from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The short contained in the String
		 *\~french
		 *\brief		Récupère un short à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le short contenu dans le String
		 */
		CU_API short to_short( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves an int from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The int contained in the String
		 *\~french
		 *\brief		Récupère un int à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		L'int contenu dans le String
		 */
		CU_API int to_int( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a long from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The long contained in the String
		 *\~french
		 *\brief		Récupère un long à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le long contenu dans le String
		 */
		CU_API long to_long( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a long long from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The long long contained in the String
		 *\~french
		 *\brief		Récupère un long long à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le long long contenu dans le String
		 */
		CU_API long long to_long_long( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a float from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The float contained in the String
		 *\~french
		 *\brief		Récupère un float à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le float contenu dans le String
		 */
		CU_API float to_float( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a double from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The double contained in the String
		 *\~french
		 *\brief		Récupère un double à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le double contenu dans le String
		 */
		CU_API double to_double( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a double from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The double contained in the String
		 *\~french
		 *\brief		Récupère un double à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le double contenu dans le String
		 */
		CU_API long double to_long_double( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a short from a given String
		 *\param[in]	p_str		The String
		 *\param[in]	p_locale	The locale used in the conversion
		 *\return		The short contained in the String
		 *\~french
		 *\brief		Récupère un short à partir d'un String
		 *\param[in]	p_str		Un String
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\return		Le short contenu dans le String
		 */
		CU_API real to_real( String const & p_str, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Tests if the given String is upper case
		 *\param[in]	p_strToTest	The String to test
		 *\return		\p true if p_strToTest is upper case
		 *\~french
		 *\brief		Teste si le String donné est en majuscules
		 *\param[in]	p_strToTest	Le String à tester
		 *\return		\p true si p_strToTest est en majuscules
		 */
		CU_API bool is_upper_case( String const & p_strToTest );
		/**
		 *\~english
		 *\brief		Tests if the given String is lower case
		 *\param[in]	p_strToTest	The String to test
		 *\return		\p true if p_strToTest is lower case
		 *\~french
		 *\brief		Teste si le String donné est en minuscules
		 *\param[in]	p_strToTest	Le String à tester
		 *\return		\p true si p_strToTest est en minuscules
		 */
		CU_API bool is_lower_case( String const & p_strToTest );
		/**
		 *\~english
		 *\brief		Sets all characters in a String to upper case
		 *\param[in]	p_str	The String to modify
		 *\return		The modified String
		 *\~french
		 *\brief		Met tous les caractères du String donné en majuscules
		 *\param[in]	p_str	Le String à modifier, reçoit le String modifié
		 *\return		Le String modifié
		 */
		CU_API String upper_case( String const & p_str );
		/**
		 *\~english
		 *\brief		Sets all characters in a String to lower case
		 *\param[in]	p_str	The String to modify
		 *\return		The modified String
		 *\~french
		 *\brief		Met tous les caractères du String donné en minuscules
		 *\param[in]	p_str	Le String à modifier, reçoit le String modifié
		 *\return		Le String modifié
		 */
		CU_API String lower_case( String const & p_str );
		/**
		 *\~english
		 *\brief			Sets all characters in a String to upper case
		 *\param[in,out]	p_str	The String to modify, receives the modified String
		 *\return			The modified String
		 *\~french
		 *\brief			Met tous les caractères du String donné en majuscules
		 *\param[in,out]	p_str	Le String à modifier, reçoit le String modifié
		 *\return			Le String modifié
		 */
		CU_API String & to_upper_case( String & p_str );
		/**
		 *\~english
		 *\brief			Sets all characters in a String to lower case
		 *\param[in,out]	p_str	The String to modify, receives the modified String
		 *\return			The modified String
		 *\~french
		 *\brief			Met tous les caractères du String donné en minuscules
		 *\param[in,out]	p_str	Le String à modifier, reçoit le String modifié
		 *\return			Le String modifié
		 */
		CU_API String & to_lower_case( String & p_str );
		/**
		 *\~english
		 *\brief		Cuts a String into substrings, using delimiter(s)
		 *\param[in]	p_str		The String to cut
		 *\param[in]	p_delims	The delimiter(s)
		 *\param[in]	p_maxSplits	The max splits count (the return will contain 0 < x < p_maxSplits substrings)
		 *\param[in]	p_bKeepVoid	Tells if the function keeps void substrings or not
		 *\return		The array containing the substrings
		 *\~french
		 *\brief		Découpe une chaîne en plusieurs sous-chaînes, en utilisant un/des délimiteur(s)
		 *\param[in]	p_str		Le String à découper
		 *\param[in]	p_delims	Le(s) délimiteur(s)
		 *\param[in]	p_maxSplits	Le nombre maximal de découpes (le retour contiendra 0 < x < p_maxSplits sous-chaînes)
		 *\param[in]	p_bKeepVoid	Dit si la fonction garde les sous-chaînes vides ou pas
		 *\return		Le tableau contenant les sous-chaînes
		 */
		CU_API StringArray split( String const & p_str, String const & p_delims, uint32_t p_maxSplits = 10, bool p_bKeepVoid = true );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a xchar by another one in a String
		 *\param[in,out]	p_str			The String to modify
		 *\param[in]		p_find			The xchar to replace
		 *\param[in]		p_replacement	The replacement xchar
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un xchar par un autre dans un String
		 *\param[in,out]	p_str			Le String à modifier
		 *\param[in]		p_find			Le xchar à remplacer
		 *\param[in]		p_replacement	Le xchar de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & p_str, xchar p_find, xchar p_replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a String by a xchar in a String
		 *\param[in,out]	p_str			The String to modify
		 *\param[in]		p_find			The String to replace
		 *\param[in]		p_replacement	The replacement xchar
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un String par un xchar dans un String
		 *\param[in,out]	p_str			Le String à modifier
		 *\param[in]		p_find			Le String à remplacer
		 *\param[in]		p_replacement	Le xchar de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & p_str, String const & p_find, xchar p_replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a xchar by a String in a String
		 *\param[in,out]	p_str			The String to modify
		 *\param[in]		p_find			The xchar to replace
		 *\param[in]		p_replacement	The replacement String
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un xchar par un String dans un String
		 *\param[in,out]	p_str			Le String à modifier
		 *\param[in]		p_find			Le xchar à remplacer
		 *\param[in]		p_replacement	Le String de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & p_str, xchar p_find, String const & p_replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a String in another one by a third one
		 *\param[in,out]	p_str			The String to modify
		 *\param[in]		p_find			The String to replace
		 *\param[in]		p_replacement	The replacement String
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un String par un autre dans un troisième
		 *\param[in,out]	p_str			Le String à modifier
		 *\param[in]		p_find			Le String à remplacer
		 *\param[in]		p_replacement	Le String de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & p_str, String const & p_find, String const & p_replacement );
		/**
		 *\~english
		 *\brief			Removes spaces on the left and/or on the right of the given String
		 *\param[in,out]	p_str		The String to trim, receives the trimmed string
		 *\param[in]		p_bLeft		Tells if we remove the left spaces
		 *\param[in]		p_bRight	Tells if we remove the right spaces
		 *\return			The trimmed String
		 *\~french
		 *\brief			Supprime les espaces à gauche et/ou à droite dans la chaîne donnée
		 *\param[in,out]	p_str		La chaîne à modifier, reçoit la chaîne modifiée
		 *\param[in]		p_bLeft		Dit si on enlève les espaces à gauche
		 *\param[in]		p_bRight	Dit si on enlève les espaces à droite
		 *\return			La chaîne sans espaces
		 */
		CU_API String & trim( String & p_str, bool p_bLeft = true, bool p_bRight = true );
		/**
		 *\~english
		 *\brief		Retrieves a value from the given String
		 *\param[in]	p_str		The String supposedly containing the value
		 *\param[in]	p_locale	The locale used in the conversion
		 *\param[out]	p_val		Receives the retrieved value
		 *\~french
		 *\brief
		 *\brief		Récupère une valeur à partir d'un String
		 *\param[in]	p_str		Le String contenant normalement la valeur
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\param[out]	p_val		Reçoit la valeur récupérée
		 *\return
		 */
		template< typename T >
		inline void parse( String const & p_str, T & p_val, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Retrieves a value from the given String
		 *\param[in]	p_str		The String supposedly containing the value
		 *\param[in]	p_locale	The locale used in the conversion
		 *\param[out]	p_val		Receives the retrieved value
		 *\~french
		 *\brief
		 *\brief		Récupère une valeur à partir d'un String
		 *\param[in]	p_str		Le String contenant normalement la valeur
		 *\param[in]	p_locale	La locale utilisée dans la conversion
		 *\param[out]	p_val		Reçoit la valeur récupérée
		 *\return
		 */
		template< typename T >
		inline void extract( String & p_str, T & p_val, std::locale const & p_locale = std::locale() );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	p_value	The value
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	p_value	La valeur
		 *\return		Le String contenant la valeur
		 */
		template< typename T >
		inline String to_string( T const & p_value, int p_base = 10, std::locale const & p_locale = std::locale( std::locale(), new manip::base_num_put< xchar >() ) );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	p_src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	p_src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > string_cast( std::basic_string< U > const & p_src );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	p_src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	p_src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_src );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	p_src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	p_src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > string_cast( std::initializer_list<  U > const & p_src );
		/*!
		\author Sylvain DOREMUS
		\version	0.8.0
		\date		21/09/2015
		\~english
		\brief 		Utf8 functions namespace
		\~french
		\brief 		Espace de nom regroupant des fonctions sur les chaînes de caractères en UTF-8
		*/
		namespace utf8
		{
			static const unsigned char FirstBitMask = 0x80; // 1000000
			static const unsigned char SecondBitMask = 0x40; // 0100000
			static const unsigned char ThirdBitMask = 0x20; // 0010000
			static const unsigned char FourthBitMask = 0x10; // 0001000
			static const unsigned char FifthBitMask = 0x08; // 0000100

			/**
			 *\~english
			 *\brief		Retrieves an UTF-8 char32_t from a char array iterator.
			 *\param[in]	p_value	The char iterator.
			 *\return		The UTF-8 char32_t character.
			 *\~french
			 *\brief		Récupère un char32_t encodé en UTF-8 à partir d'un itérateur sur une chaîne.
			 *\param[in]	p_value	L'itérateur sur la chaîne.
			 *\return		Le caractère char32_t en UTF-8.
			*/
			template< typename IteratorType >
			inline char32_t to_utf8( IteratorType p_value );
			/*!
			\author		Sylvain Doremus
			\version	0.8.0
			\date		09/03/2015
			\~english
			\brief		An iterator, to an UTF-8 string
			\~french
			\brief		Un itérateur sur une chaîne UTF-8
			*/
			class iterator
				: public std::iterator< std::bidirectional_iterator_tag, char32_t, std::string::difference_type, const char32_t *, const char32_t & >
			{
			public:
				/**
				 *\~english
				 *\brief		Constructor.
				 *\param[in]	p_it	The string iterator.
				 *\~french
				 *\brief		Constructeur.
				 *\param[in]	p_it	L'itérateur sur la chaîne.
				*/
				inline iterator( std::string::const_iterator const & p_it );
				/**
				 *\~english
				 *\brief		Copy constructor.
				 *\param[in]	p_it	The other object.
				 *\~french
				 *\brief		Constructeur par copie.
				 *\param[in]	p_it	L'autre objet.
				*/
				inline iterator( iterator const & p_it );
				/**
				 *\~english
				 *\brief		Destructor
				 *\~french
				 *\brief		Destructeur.
				*/
				inline ~iterator();
				/**
				 *\~english
				 *\brief		Assignment operator.
				 *\param[in]	p_it	The string iterator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	p_it	L'itérateur sur la chaîne originale.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator=( std::string::const_iterator const & p_it );
				/**
				 *\~english
				 *\brief		Assignment operator.
				 *\param[in]	p_it	The string iterator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	p_it	L'itérateur sur la chaîne originale.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator=( std::string::iterator const & p_it );
				/**
				 *\~english
				 *\brief		Copy assignment operator.
				 *\param[in]	p_it	The other object.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	p_it	L'autre itérateur.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator=( iterator const & p_it );
				/**
				 *\~english
				 *\brief		Assigment addition operator.
				 *\param[in]	p_offset	The offset to add.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affection par addition.
				 *\param[in]	p_offset	Le décalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator+=( size_t p_offset );
				/**
				 *\~english
				 *\brief		Assignment subtraction operator.
				 *\param[in]	p_offset	The offset to subtract.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation par soustraction.
				 *\param[in]	p_offset	Ld eécalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator-=( size_t p_offset );
				/**
				 *\~english
				 *\brief		Pre-increment operator.
				 \return		A reference to this object.
				 *\~english
				 *\brief		Opérateur de pré-increment.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator++();
				/**
				 *\~english
				 *\brief		Post-increment operator.
				 \return		A copy of this iterator, before increment.
				 *\~french
				 *\brief		Opérateur de post-increment.
				 \return		Une copie de cet itérateur, avant l'incrémentation.
				*/
				inline iterator operator++( int );
				/**
				 *\~english
				 *\brief		Pre-decrement operator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur de pré-décrement.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator--();
				/**
				 *\~english
				 *\brief		Post-decrement operator.
				 \return		A copy of this iterator, before decrement.
				 *\~french
				 *\brief		Opérateur de post-décrement.
				 \return		Une copie de cet itérateur, avant la décrémentation.
				*/
				inline iterator operator--( int );
				/**
				 *\~english
				 *\brief		Access to the current codepoint value.
				 \return		The current codepoint value.
				 *\~french
				 *\brief		Accès à la valeur UTF-8 courante.
				 \return		La valeur.
				*/
				inline char32_t operator*()const;
				/**
				 *\~english
				 *\brief		Equality operator.
				 *\~french
				 *\brief		Opérateur d'égalité.
				*/
				inline bool operator==( const iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Equality operator.
				 *\~french
				 *\brief		Opérateur d'égalité.
				*/
				inline bool operator==( const std::string::const_iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Equality operator.
				 *\~french
				 *\brief		Opérateur d'égalité.
				*/
				inline bool operator==( const std::string::iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const std::string::const_iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const std::string::iterator & p_it )const;
				/**
				 *\~english
				 *\brief		Retrieves the internal iterator.
				 *\~french
				 *\brief		Récupère la'itérateur interne.
				*/
				inline std::string::const_iterator internal()const;

			private:
				/**
				 *\~english
				 *\brief		Computes the cached codepoint.
				 *\~french
				 *\brief		Calcule la valeur UTF-8 cachée.
				*/
				inline void DoCalculateCurrentCodePoint()const;

			private:
				//!\~english The internal iterator.
				std::string::const_iterator m_it;
				//!\~english The last computed codepoint.
				mutable char32_t m_lastCodePoint;
				//!\~english Tells the codepoint needs recomputing.
				mutable bool m_dirty;
			};
			/**
			 *\~english
			 *\brief		Addition operator.
			 *\param[in]	p_it		The iterator.
			 *\param[in]	p_offset	The offset to add.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Addition operator.
			 *\param[in]	p_it		The iterator.
			 *\param[in]	p_offset	The offset to add.
			 \return		A reference to this object.
			*/
			inline iterator operator+( iterator p_it, size_t p_offset );
			/**
			 *\~english
			 *\brief		Subtraction operator.
			 *\param[in]	p_it		The iterator.
			 *\param[in]	p_offset	The offset to subtract.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Subtraction operator.
			 *\param[in]	p_it		The iterator.
			 *\param[in]	p_offset	The offset to subtract.
			 \return		A reference to this object.
			*/
			inline iterator operator-( iterator p_it, size_t p_offset );
		}
	}
	/*!
	\author 	Sylvain DOREMUS
	\date 		19/06/2012
	\~english
	\brief		An empty String
	\~french
	\brief		Une chaîne vide
	*/
	static const String cuEmptyString;
}

#include "StringUtils.inl"

#endif
