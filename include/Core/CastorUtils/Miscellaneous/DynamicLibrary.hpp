/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_DYNAMIC_LIBRARY_H___
#define ___CASTOR_DYNAMIC_LIBRARY_H___

#include "CastorUtils/Data/Path.hpp"

namespace castor
{
	using VoidFnType = void( CU_stdcall * )( );
	class DynamicLibrary
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor, loads the library.
		 *\param[in]	pathFile	The file path
		 *\~french
		 *\brief		Constructeur, charge la bibliothèque
		 *\param[in]	pathFile	Le chemin du fichier
		 */
		CU_API explicit DynamicLibrary( Path const & pathFile )noexcept;
		/**
		 *\~english
		 *\brief		Constructor, loads the library.
		 *\param[in]	pathFile	The file path
		 *\~french
		 *\brief		Constructeur, charge la bibliothèque
		 *\param[in]	pathFile	Le chemin du fichier
		 */
		CU_API explicit DynamicLibrary( String const & pathFile )noexcept;
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	lib	The DynamicLibrary to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	lib	La DynamicLibrary à copier
		 */
		CU_API DynamicLibrary( DynamicLibrary const & lib )noexcept;
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	lib	The DynamicLibrary to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	lib	La DynamicLibrary à déplacer
		 */
		CU_API DynamicLibrary( DynamicLibrary && lib )noexcept;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~DynamicLibrary()noexcept;
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	lib	The DynamicLibrary to copy
		 *\return		A reference to this DynamicLibrary
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	lib	La DynamicLibrary à copier
		 *\return		Une référence sur cette DynamicLibrary
		 */
		CU_API DynamicLibrary & operator=( DynamicLibrary const & lib );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	lib	The DynamicLibrary to move
		 *\return		A reference to this DynamicLibrary
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	lib	La DynamicLibrary à déplacer
		 *\return		Une référence sur cette DynamicLibrary
		 */
		CU_API DynamicLibrary & operator=( DynamicLibrary && lib )noexcept;
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & function, char const * name )noexcept
		{
			return getFunction( function, makeString( name ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & function, wchar_t const * name )noexcept
		{
			return getFunction( function, makeString( name ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & function, String const & name )noexcept
		{
			function = FuncType( doGetFunction( name ) );
			return function != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( char const * name, FuncType & function )noexcept
		{
			return getFunction( function, makeString( name ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( wchar_t const * name, FuncType & function )noexcept
		{
			return getFunction( function, makeString( name ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	function	Receives the function
		 *\param[in]	name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	function	Reçoit la fonction
		 *\param[in]	name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( String const & name, FuncType & function )noexcept
		{
			function = FuncType( doGetFunction( name ) );
			return function != nullptr;
		}
		/**
		 *\~english
		 *\brief		Tells if the DynamicLibrary is opened (loaded)
		 *\return		\p true if it is opened, \p false if not
		 *\~french
		 *\brief		Dit si la DynamicLibrary est ouverte (chargée)
		 *\return		\p true si elle est ouverte, \p false sinon
		 */
		inline bool isOpen()const
		{
			return m_library != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves the file path
		 *\return		The file path
		 *\~french
		 *\brief		Récupère le chemin vers le fichier
		 *\return		Le chemin
		 */
		inline Path const & getPath()const
		{
			return m_pathLibrary;
		}

	private:
		CU_API void doOpen()noexcept;
		CU_API void doClose()noexcept;
		CU_API VoidFnType doGetFunction( String const & name )noexcept;

	private:
		void * m_library{};
		Path m_pathLibrary{};
	};
}

#endif
