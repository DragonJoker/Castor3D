/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PATH_H___
#define ___CASTOR_PATH_H___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	class Path
		: public String
	{
	public:
		//!\~english	Path separator (OS dependant).
		//!\~french		Séparateur de chemin (dépendant de l'OS)
		CU_API static const xchar NativeSeparator;
		//!\~english	Path separator (OS independant).
		//!\~french		Séparateur de chemin (indépendant de l'OS)
		CU_API static const xchar GenericSeparator;

	public:
		CU_API Path();
		/**
		 *\~english
		 *\brief		Constructor from a char array
		 *\param[in]	data	The char array
		 *\~french
		 *\brief		Constructeur à partir d'un tableau de char
		 *\param[in]	data	Le tableau de char
		 */
		CU_API explicit Path( char const * data );
		/**
		 *\~english
		 *\brief		Constructor from a wchar_t array
		 *\param[in]	data	The wchar_t array
		 *\~french
		 *\brief		Constructeur à partir d'un tableau de wchar_t
		 *\param[in]	data	Le tableau de wchar_t
		 */
		CU_API explicit Path( wchar_t const * data );
		/**
		 *\~english
		 *\brief		Constructor from a String
		 *\param[in]	data	The String
		 *\~french
		 *\brief		Constructeur à partir d'un String
		 *\param[in]	data	Le String
		 */
		CU_API explicit Path( String const & data );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	data	The Path object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	data	L'objet Path à copier
		 */
		CU_API Path( Path const & data );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	data	The Path object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	data	L'objet Path à déplacer
		 */
		CU_API Path( Path && data );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	data	The Path object to copy
		 *\return		A reference to this Path object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	data	L'objet Path à copier
		 *\return		Une référence sur cet objet Path
		 */
		CU_API Path & operator=( Path const & data );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	data	The Path object to move
		 *\return		A reference to this Path object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	data	L'objet Path à déplacer
		 *\return		Une référence sur cet objet Path
		 */
		CU_API Path & operator=( Path && data );
		/**
		 *\~english
		 *\brief		Appends a path to the current path. adds the separator if needed
		 *\param[in]	path	The path to add
		 *\~french
		 *\brief		Concatène un chemin à celui-ci. Ajoute le séparateur si besoin est
		 *\param[in]	path	Le chemin à concaténer
		 */
		CU_API Path & operator/=( Path const & path );
		/**
		 *\~english
		 *\brief		adds a string to the current path. Adds the separator if needed
		 *\param[in]	string	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	string	La chaîne de caractères à concaténer
		 */
		CU_API Path & operator/=( String const & string );
		/**
		 *\~english
		 *\brief		adds a MBCS C string to the current path. Adds the separator if needed
		 *\param[in]	buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C MBCS à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator/=( char const * buffer );
		/**
		 *\~english
		 *\brief		adds a Unicode C string to the current path. Adds the separator if needed
		 *\param[in]	buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C Unicode à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator/=( wchar_t const * buffer );
		/**
		 *\~english
		 *\brief		Appends a path to the current path.
		 *\param[in]	path	The path to add
		 *\~french
		 *\brief		Concatène un chemin à celui-ci.
		 *\param[in]	path	Le chemin à concaténer
		 */
		CU_API Path & operator+=( Path const & path );
		/**
		 *\~english
		 *\brief		adds a string to the current path.
		 *\param[in]	string	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères à ce chemin.
		 *\param[in]	string	La chaîne de caractères à concaténer
		 */
		CU_API Path & operator+=( String const & string );
		/**
		 *\~english
		 *\brief		adds a MBCS C string to the current path.
		 *\param[in]	buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C MBCS à ce chemin.
		 *\param[in]	buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator+=( char const * buffer );
		/**
		 *\~english
		 *\brief		adds a Unicode C string to the current path.
		 *\param[in]	buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C Unicode à ce chemin.
		 *\param[in]	buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator+=( wchar_t const * buffer );
		/**
		 *\~english
		 *\brief		Retrieves the parent path
		 *\return		The parent path
		 *\~french
		 *\brief		Récupère le dossier parent
		 *\return		Le dossier parent
		 */
		CU_API Path getPath()const;
		/**
		 *\~english
		 *\brief		Retrieves the leaf of this path (filename and extension for files).
		 *\param[in]	withExtension	Tells if we want extension or not, for files.
		 *\return		The leaf.
		 *\~french
		 *\brief		Récupère la feuille de ce chemin (nom du fichier et extension, pour les fichiers).
		 *\param[in]	withExtension	Dit si on veur l'extension ou pas, pour les fichiers.
		 *\return		La feuille.
		 */
		CU_API Path getFileName( bool withExtension = false )const;
		/**
		 *\~english
		 *\brief		Retrieves the full path and leaf
		 *\return		The full path and leaf
		 *\~french
		 *\brief		Récupère le chemin complet
		 *\return		Le chemin complet
		 */
		CU_API Path getFullFileName()const;
		/**
		 *\~english
		 *\brief		Retrieves the extension of the file name, if any
		 *\return		The extension
		 *\~french
		 *\brief		Récupère l'extension du nom de fichier, s'il y en a
		 *\return		L'extension
		 */
		CU_API String getExtension()const;
		/**
		 *\~english
		 *\return		The file path, with generic /.
		 *\~french
		 *\return		Le chemin, avec des / génériques.
		 */
		CU_API String toGeneric()const;

	private:
		void doNormalise();
	};
	/**
	 *\~english
	 *\brief		Appends 2 paths. adds the separator if needed
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	The second path to add
	 *\~french
	 *\brief		Concatène 2 chemins. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	Le premier chemin à concaténer
	 *\param[in]	rhs	Le second chemin à concaténer
	 */
	CU_API Path operator/( Path const & lhs, Path const & rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & lhs, String const & rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & lhs, char const * rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	The first path to add
	 *\param[in]	rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & lhs, wchar_t const * rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The path to add
	 *\param[in]	rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	La chaîne de caractères à concaténer
	 *\param[in]	rhs	The first path to add
	 */
	CU_API Path operator/( String const & lhs, Path const & rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The path to add
	 *\param[in]	rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	La chaîne de caractères à concaténer
	 *\param[in]	rhs	The first path to add
	 */
	CU_API Path operator/( char const * lhs, Path const & rhs );
	/**
	 *\~english
	 *\brief		adds a string to a path. Adds the separator if needed
	 *\param[in]	lhs	The path to add
	 *\param[in]	rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	lhs	La chaîne de caractères à concaténer
	 *\param[in]	rhs	The first path to add
	 */
	CU_API Path operator/( wchar_t const * lhs, Path const & rhs );
}

#endif
