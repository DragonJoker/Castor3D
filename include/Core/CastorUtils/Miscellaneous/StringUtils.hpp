/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STRING_H___
#define ___CASTOR_STRING_H___

#include "CastorUtils/Miscellaneous/MiscellaneousModule.hpp"

#include "CastorUtils/Stream/StreamBaseManipulators.hpp"
#include <sstream>

namespace castor
{
	namespace string
	{
		/**
		 *\~english
		 *\brief		Tests if the given String represents an integer
		 *\param[in]	toTest	The String to test
		 *\param[in]	locale	The locale (unused)
		 *\return		\p false if toTest contains any character other than '0123456789'
		 *\~french
		 *\brief		Teste si le String donné réprésente un nombre entier
		 *\param[in]	toTest Le String à tester
		 *\param[in]	locale	La locale (inutilisée)
		 *\return		\p false si toTest contient n'importe quel caractère autre que '0123456789'
		 */
		CU_API bool isInteger( String const & toTest, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Tests if the given String represents a floating number
		 *\param[in]	toTest	The String to test
		 *\param[in]	locale	The locale used to determine the dot format
		 *\return		\p false if toTest contains any character other than '0123456789.,' or more than one '.'/','
		 *\~french
		 *\brief		Teste si le String donné réprésente un nombre flottant
		 *\param[in]	toTest Le String à tester
		 *\param[in]	locale	La locale utilisée pour déterminer le format du séparateur '.'
		 *\return		\p false si toTest contient n'importe quel caractère autre que '0123456789.,' ou plus d'un '.'/','
		 */
		CU_API bool isFloating( String const & toTest, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a short from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The short contained in the String
		 *\~french
		 *\brief		Récupère un short à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le short contenu dans le String
		 */
		CU_API short toShort( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves an int from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The int contained in the String
		 *\~french
		 *\brief		Récupère un int à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		L'int contenu dans le String
		 */
		CU_API int toInt( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a long from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The long contained in the String
		 *\~french
		 *\brief		Récupère un long à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le long contenu dans le String
		 */
		CU_API long toLong( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a long long from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The long long contained in the String
		 *\~french
		 *\brief		Récupère un long long à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le long long contenu dans le String
		 */
		CU_API long long toLongLong( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves an unsigned short from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The unsigned short contained in the String
		 *\~french
		 *\brief		Récupère un unsigned short à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		L'unsigned short contenu dans le String
		 */
		CU_API unsigned short toUShort( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves an unsigned int from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The unsigned int contained in the String
		 *\~french
		 *\brief		Récupère un unsigned int à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		L'unsigned int contenu dans le String
		 */
		CU_API unsigned int toUInt( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves an unsigned long from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The unsigned long contained in the String
		 *\~french
		 *\brief		Récupère un unsigned long à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		L'unsigned long contenu dans le String
		 */
		CU_API unsigned long toULong( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves an unsigned long long from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The unsigned long long contained in the String
		 *\~french
		 *\brief		Récupère un unsigned long long à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		L'unsigned long long contenu dans le String
		 */
		CU_API unsigned long long toULongLong( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a float from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The float contained in the String
		 *\~french
		 *\brief		Récupère un float à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le float contenu dans le String
		 */
		CU_API float toFloat( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a double from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The double contained in the String
		 *\~french
		 *\brief		Récupère un double à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le double contenu dans le String
		 */
		CU_API double toDouble( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a double from a given String
		 *\param[in]	str		The String
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The double contained in the String
		 *\~french
		 *\brief		Récupère un double à partir d'un String
		 *\param[in]	str		Un String
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le double contenu dans le String
		 */
		CU_API long double toLongDouble( String const & str, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Tests if the given String is upper case
		 *\param[in]	toTest	The String to test
		 *\return		\p true if toTest is upper case
		 *\~french
		 *\brief		Teste si le String donné est en majuscules
		 *\param[in]	toTest	Le String à tester
		 *\return		\p true si toTest est en majuscules
		 */
		CU_API bool isUpperCase( String const & toTest );
		/**
		 *\~english
		 *\brief		Tests if the given String is lower case
		 *\param[in]	toTest	The String to test
		 *\return		\p true if toTest is lower case
		 *\~french
		 *\brief		Teste si le String donné est en minuscules
		 *\param[in]	toTest	Le String à tester
		 *\return		\p true si toTest est en minuscules
		 */
		CU_API bool isLowerCase( String const & toTest );
		/**
		 *\~english
		 *\brief		sets all characters in a String to upper case
		 *\param[in]	str	The String to modify
		 *\return		The modified String
		 *\~french
		 *\brief		Met tous les caractères du String donné en majuscules
		 *\param[in]	str	Le String à modifier, reçoit le String modifié
		 *\return		Le String modifié
		 */
		CU_API String upperCase( String const & str );
		/**
		 *\~english
		 *\brief		sets all characters in a String to lower case
		 *\param[in]	str	The String to modify
		 *\return		The modified String
		 *\~french
		 *\brief		Met tous les caractères du String donné en minuscules
		 *\param[in]	str	Le String à modifier, reçoit le String modifié
		 *\return		Le String modifié
		 */
		CU_API String lowerCase( String const & str );
		/**
		 *\~english
		 *\brief			Converts given string from snake_case to CamelCase.
		 *\~french
		 *\brief			Convertit la chaine donnée de snake_case vers CamelCase.
		 */
		CU_API String snakeToCamelCase( String str );
		/**
		 *\~english
		 *\brief			sets all characters in a String to upper case
		 *\param[in,out]	str	The String to modify, receives the modified String
		 *\return			The modified String
		 *\~french
		 *\brief			Met tous les caractères du String donné en majuscules
		 *\param[in,out]	str	Le String à modifier, reçoit le String modifié
		 *\return			Le String modifié
		 */
		CU_API String & toUpperCase( String & str );
		/**
		 *\~english
		 *\brief			sets all characters in a String to lower case
		 *\param[in,out]	str	The String to modify, receives the modified String
		 *\return			The modified String
		 *\~french
		 *\brief			Met tous les caractères du String donné en minuscules
		 *\param[in,out]	str	Le String à modifier, reçoit le String modifié
		 *\return			Le String modifié
		 */
		CU_API String & toLowerCase( String & str );
		/**
		 *\~english
		 *\brief		Cuts a String into substrings, using delimiter(s)
		 *\param[in]	str			The String to cut
		 *\param[in]	delims		The delimiter(s)
		 *\param[in]	maxSplits	The max splits count (the return will contain 0 < x < maxSplits substrings)
		 *\param[in]	keepEmpty	Tells if the function keeps empty substrings or not
		 *\return		The array containing the substrings
		 *\~french
		 *\brief		Découpe une chaîne en plusieurs sous-chaînes, en utilisant un/des délimiteur(s)
		 *\param[in]	str			Le String à découper
		 *\param[in]	delims		Le(s) délimiteur(s)
		 *\param[in]	maxSplits	Le nombre maximal de découpes (le retour contiendra 0 < x < maxSplits sous-chaînes)
		 *\param[in]	keepEmpty	Dit si la fonction garde les sous-chaînes vides ou pas
		 *\return		Le tableau contenant les sous-chaînes
		 */
		CU_API StringArray split( String const & str, String const & delims, uint32_t maxSplits = 10, bool keepEmpty = true );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a xchar by another one in a String
		 *\param[in,out]	str			The String to modify
		 *\param[in]		toFind		The xchar to replace
		 *\param[in]		replacement	The replacement xchar
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un xchar par un autre dans un String
		 *\param[in,out]	str			Le String à modifier
		 *\param[in]		toFind		Le xchar à remplacer
		 *\param[in]		replacement	Le xchar de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & str, xchar toFind, xchar replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a String by a xchar in a String
		 *\param[in,out]	str			The String to modify
		 *\param[in]		toFind		The String to replace
		 *\param[in]		replacement	The replacement xchar
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un String par un xchar dans un String
		 *\param[in,out]	str			Le String à modifier
		 *\param[in]		toFind		Le String à remplacer
		 *\param[in]		replacement	Le xchar de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & str, String const & toFind, xchar replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a xchar by a String in a String
		 *\param[in,out]	str			The String to modify
		 *\param[in]		toFind		The xchar to replace
		 *\param[in]		replacement	The replacement String
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un xchar par un String dans un String
		 *\param[in,out]	str			Le String à modifier
		 *\param[in]		toFind		Le xchar à remplacer
		 *\param[in]		replacement	Le String de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & str, xchar toFind, String const & replacement );
		/**
		 *\~english
		 *\brief			Replaces all occurences of a String in another one by a third one
		 *\param[in,out]	str			The String to modify
		 *\param[in]		toFind		The String to replace
		 *\param[in]		replacement	The replacement String
		 *\return			A reference on the modified string
		 *\~french
		 *\brief			Remplace toutes les occurences d'un String par un autre dans un troisième
		 *\param[in,out]	str			Le String à modifier
		 *\param[in]		toFind		Le String à remplacer
		 *\param[in]		replacement	Le String de remplacement
		 *\return			Une référence sur la chaîne modifiée
		 */
		CU_API String & replace( String & str, String const & toFind, String const & replacement );
		/**
		 *\~english
		 *\brief			Removes spaces on the left and/or on the right of the given String
		 *\param[in,out]	str		The String to trim, receives the trimmed string
		 *\param[in]		left	Tells if we remove the left spaces
		 *\param[in]		right	Tells if we remove the right spaces
		 *\return			The trimmed String
		 *\~french
		 *\brief			Supprime les espaces à gauche et/ou à droite dans la chaîne donnée
		 *\param[in,out]	str		La chaîne à modifier, reçoit la chaîne modifiée
		 *\param[in]		left	Dit si on enlève les espaces à gauche
		 *\param[in]		right	Dit si on enlève les espaces à droite
		 *\return			La chaîne sans espaces
		 */
		CU_API String & trim( String & str, bool left = true, bool right = true );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( char32_t value );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( uint8_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( int16_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( uint16_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( int32_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( uint32_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( int64_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	base	The numeric base
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	base	La base numérique
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		CU_API String toString( uint64_t value, int base = 10, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Retrieves a value from the given String
		 *\param[in]	str		The String supposedly containing the value
		 *\param[in]	locale	The locale used in the conversion
		 *\param[out]	value	Receives the retrieved value
		 *\~french
		 *\brief
		 *\brief		Récupère une valeur à partir d'un String
		 *\param[in]	str		Le String contenant normalement la valeur
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\param[out]	value	Reçoit la valeur récupérée
		 */
		template< typename T >
		inline void parse( String const & str, T & value, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Retrieves a value from the given String
		 *\param[in]	str		The String supposedly containing the value
		 *\param[out]	value	Receives the retrieved value
		 *\param[in]	locale	The locale used in the conversion
		 *\~french
		 *\brief
		 *\brief		Récupère une valeur à partir d'un String
		 *\param[in]	str		Le String contenant normalement la valeur
		 *\param[out]	value	Reçoit la valeur récupérée
		 *\param[in]	locale	La locale utilisée dans la conversion
		 */
		template< typename T >
		inline void extract( String & str, T & value, std::locale const & locale = std::locale( "C" ) );
		/**
		 *\~english
		 *\brief		Puts a value into a String
		 *\param[in]	value	The value
		 *\param[in]	locale	The locale used in the conversion
		 *\return		The String containing the value
		 *\~french
		 *\brief		Met une valeur dans un String
		 *\param[in]	value	La valeur
		 *\param[in]	locale	La locale utilisée dans la conversion
		 *\return		Le String contenant la valeur
		 */
		template< typename T >
		inline String toString( T const & value, std::locale const & locale = std::locale( std::locale( "C" ), new manip::BaseNumPut< xchar >() ) );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > stringCast( std::basic_string< U > const & src );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > stringCast( U const * src );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type.
		 *\param[in]	begin	The source value.
		 *\param[in]	end		The source end.
		 *\return		The cast string.
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne.
		 *\param[in]	begin	La valeur source.
		 *\param[in]	end		La fin de la source.
		 *\return		La chaîne convertie.
		 */
		template< typename T, typename U >
		std::basic_string< T > stringCast( U const * begin, U const * end );
		/**
		 *\~english
		 *\brief		Casts a string value in another string type
		 *\param[in]	src	The source value
		 *\return		The cast string
		 *\~french
		 *\brief		Convertit une chaîne dans un autre type de chaîne
		 *\param[in]	src	La valeur source
		 *\return		La chaîne convertie
		 */
		template< typename T, typename U >
		std::basic_string< T > stringCast( std::initializer_list<  U > const & src );

		namespace utf8
		{
			/**
			 *\~english
			 *\brief		Retrieves an UTF-8 char32_t from a char array iterator.
			 *\param[in]	value	The char iterator.
			 *\return		The UTF-8 char32_t character.
			 *\~french
			 *\brief		Récupère un char32_t encodé en UTF-8 à partir d'un itérateur sur une chaîne.
			 *\param[in]	value	L'itérateur sur la chaîne.
			 *\return		Le caractère char32_t en UTF-8.
			*/
			template< typename IteratorType >
			inline char32_t toUtf8( IteratorType value );

			class iterator
			{
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using value_type = char32_t;
				using difference_type = std::string::difference_type;
				using pointer = const char32_t *;
				using reference = const char32_t &;

			public:
				/**
				 *\~english
				 *\brief		Constructor.
				 *\param[in]	it	The string iterator.
				 *\~french
				 *\brief		Constructeur.
				 *\param[in]	it	L'itérateur sur la chaîne.
				*/
				inline explicit iterator( std::string::iterator const & it );
				/**
				 *\~english
				 *\brief		Copy constructor.
				 *\param[in]	it	The other object.
				 *\~french
				 *\brief		Constructeur par copie.
				 *\param[in]	it	L'autre objet.
				*/
				inline iterator( iterator const & it );
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
				 *\param[in]	it	The string iterator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	it	L'itérateur sur la chaîne originale.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator=( std::string::iterator const & it );
				/**
				 *\~english
				 *\brief		Copy assignment operator.
				 *\param[in]	it	The other object.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	it	L'autre itérateur.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator=( iterator const & it );
				/**
				 *\~english
				 *\brief		Assigment addition operator.
				 *\param[in]	offset	The offset to add.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affection par addition.
				 *\param[in]	offset	Le décalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator+=( size_t offset );
				/**
				 *\~english
				 *\brief		Assignment subtraction operator.
				 *\param[in]	offset	The offset to subtract.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation par soustraction.
				 *\param[in]	offset	Le décalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline iterator & operator-=( size_t offset );
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
				inline bool operator==( const iterator & it )const;
				/**
				 *\~english
				 *\brief		Equality operator.
				 *\~french
				 *\brief		Opérateur d'égalité.
				*/
				inline bool operator==( const std::string::iterator & it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const iterator & it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const std::string::iterator & it )const;
				/**
				 *\~english
				 *\brief		Retrieves the internal iterator.
				 *\~french
				 *\brief		Récupère la'itérateur interne.
				*/
				inline std::string::iterator internal()const;

			private:
				/**
				 *\~english
				 *\brief		Computes the cached codepoint.
				 *\~french
				 *\brief		Calcule la valeur UTF-8 cachée.
				*/
				inline void doCalculateCurrentCodePoint()const;

			private:
				//!\~english The internal iterator.
				std::string::iterator m_it;
				//!\~english The last computed codepoint.
				mutable char32_t m_lastCodePoint;
				//!\~english Tells the codepoint needs recomputing.
				mutable bool m_dirty;
			};
			/**
			 *\~english
			 *\brief		addition operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to add.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Addition operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to add.
			 \return		A reference to this object.
			*/
			inline iterator operator+( iterator it, size_t offset );
			/**
			 *\~english
			 *\brief		Subtraction operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to subtract.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Subtraction operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to subtract.
			 \return		A reference to this object.
			*/
			inline iterator operator-( iterator it, size_t offset );

			class const_iterator
			{
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using value_type = char32_t;
				using difference_type = std::string::difference_type;
				using pointer = const char32_t *;
				using reference = const char32_t &;

			public:
				/**
				 *\~english
				 *\brief		Constructor.
				 *\param[in]	it	The string iterator.
				 *\~french
				 *\brief		Constructeur.
				 *\param[in]	it	L'itérateur sur la chaîne.
				*/
				inline explicit const_iterator( std::string::const_iterator const & it );
				/**
				 *\~english
				 *\brief		Copy constructor.
				 *\param[in]	it	The other object.
				 *\~french
				 *\brief		Constructeur par copie.
				 *\param[in]	it	L'autre objet.
				*/
				inline const_iterator( const_iterator const & it );
				/**
				 *\~english
				 *\brief		Destructor
				 *\~french
				 *\brief		Destructeur.
				*/
				inline ~const_iterator();
				/**
				 *\~english
				 *\brief		Assignment operator.
				 *\param[in]	it	The string iterator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	it	L'itérateur sur la chaîne originale.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator=( std::string::const_iterator const & it );
				/**
				 *\~english
				 *\brief		Copy assignment operator.
				 *\param[in]	it	The other object.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation.
				 *\param[in]	it	L'autre itérateur.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator=( const_iterator const & it );
				/**
				 *\~english
				 *\brief		Assigment addition operator.
				 *\param[in]	offset	The offset to add.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affection par addition.
				 *\param[in]	offset	Le décalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator+=( size_t offset );
				/**
				 *\~english
				 *\brief		Assignment subtraction operator.
				 *\param[in]	offset	The offset to subtract.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur d'affectation par soustraction.
				 *\param[in]	offset	Ld eécalage à appliquer.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator-=( size_t offset );
				/**
				 *\~english
				 *\brief		Pre-increment operator.
				 \return		A reference to this object.
				 *\~english
				 *\brief		Opérateur de pré-increment.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator++();
				/**
				 *\~english
				 *\brief		Post-increment operator.
				 \return		A copy of this iterator, before increment.
				 *\~french
				 *\brief		Opérateur de post-increment.
				 \return		Une copie de cet itérateur, avant l'incrémentation.
				*/
				inline const_iterator operator++( int );
				/**
				 *\~english
				 *\brief		Pre-decrement operator.
				 \return		A reference to this object.
				 *\~french
				 *\brief		Opérateur de pré-décrement.
				 \return		Une référence sur cet objet.
				*/
				inline const_iterator & operator--();
				/**
				 *\~english
				 *\brief		Post-decrement operator.
				 \return		A copy of this iterator, before decrement.
				 *\~french
				 *\brief		Opérateur de post-décrement.
				 \return		Une copie de cet itérateur, avant la décrémentation.
				*/
				inline const_iterator operator--( int );
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
				inline bool operator==( const const_iterator & it )const;
				/**
				 *\~english
				 *\brief		Equality operator.
				 *\~french
				 *\brief		Opérateur d'égalité.
				*/
				inline bool operator==( const std::string::const_iterator & it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const const_iterator & it )const;
				/**
				 *\~english
				 *\brief		Difference operator.
				 *\~french
				 *\brief		Opérateur de différence.
				*/
				inline bool operator!=( const std::string::const_iterator & it )const;
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
				inline void doCalculateCurrentCodePoint()const;

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
			 *\brief		addition operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to add.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Addition operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to add.
			 \return		A reference to this object.
			*/
			inline const_iterator operator+( const_iterator it, size_t offset );
			/**
			 *\~english
			 *\brief		Subtraction operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to subtract.
			 \return		A reference to this object.
			 *\~french
			 *\brief		Subtraction operator.
			 *\param[in]	it		The iterator.
			 *\param[in]	offset	The offset to subtract.
			 \return		A reference to this object.
			*/
			inline const_iterator operator-( const_iterator it, size_t offset );
		}
	}
}

#include "StringUtils.inl"

#endif
