/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_DYNAMIC_LIBRARY_H___
#define ___CASTOR_DYNAMIC_LIBRARY_H___

#include "CastorUtils.hpp"
#include "Data/Path.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Platform independant library
	\remark		Loads a library and gives access to it's functions in a platform independant way
	\~french
	\brief		bibliothèque dynamique (Dll, shared lib)
	\remark		Charge une bibliothèque et permet l'accès a ses fonction de manière indépendante de l'OS
	*/
	class DynamicLibrary
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CU_API DynamicLibrary()throw();
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_lib	The DynamicLibrary to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_lib	La DynamicLibrary à copier
		 */
		CU_API DynamicLibrary( DynamicLibrary const & p_lib )throw();
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_lib	The DynamicLibrary to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_lib	La DynamicLibrary à déplacer
		 */
		CU_API DynamicLibrary( DynamicLibrary && p_lib )throw();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~DynamicLibrary()throw();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_lib	The DynamicLibrary to copy
		 *\return		A reference to this DynamicLibrary
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_lib	La DynamicLibrary à copier
		 *\return		Une référence sur cette DynamicLibrary
		 */
		CU_API DynamicLibrary & operator =( DynamicLibrary const & p_lib );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_lib	The DynamicLibrary to move
		 *\return		A reference to this DynamicLibrary
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_lib	La DynamicLibrary à déplacer
		 *\return		Une référence sur cette DynamicLibrary
		 */
		CU_API DynamicLibrary & operator =( DynamicLibrary && p_lib );
		/**
		 *\~english
		 *\brief		Opens a library from a path
		 *\param[in]	p_szPath	The file path
		 *\return		\p true if library was successfully loaded
		 *\~french
		 *\brief		Charge une bibliothèque à partir d'un chemin
		 *\param[in]	p_szPath	Le chemin du fichier
		 *\return		\p true si la librarie s'est chargée correctement
		 */
		CU_API bool open( xchar const * p_szPath )throw();
		/**
		 *\~english
		 *\brief		Opens a library from a path
		 *\param[in]	p_strPath	The file path
		 *\return		\p true if library was successfully loaded
		 *\~french
		 *\brief		Charge une bibliothèque à partir d'un chemin
		 *\param[in]	p_strPath	Le chemin du fichier
		 *\return		\p true si la librarie s'est chargée correctement
		 */
		CU_API bool open( String const & p_strPath )throw();
		/**
		 *\~english
		 *\brief		Opens a library from a path
		 *\param[in]	p_pathFile	The file path
		 *\return		\p true if library was successfully loaded
		 *\~french
		 *\brief		Charge une bibliothèque à partir d'un chemin
		 *\param[in]	p_pathFile	Le chemin du fichier
		 *\return		\p true si la librarie s'est chargée correctement
		 */
		CU_API bool open( Path const & p_pathFile )throw();
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	p_pfnFunction	Receives the function
		 *\param[in]	p_szName		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	p_pfnFunction	Reçoit la fonction
		 *\param[in]	p_szName		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & p_pfnFunction, char const * p_szName )throw()
		{
			return getFunction( p_pfnFunction, string::stringCast< xchar >( p_szName ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	p_pfnFunction	Receives the function
		 *\param[in]	p_wszName		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	p_pfnFunction	Reçoit la fonction
		 *\param[in]	p_wszName		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & p_pfnFunction, wchar_t const * p_wszName )throw()
		{
			return getFunction( p_pfnFunction, string::stringCast< xchar >( p_wszName ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves a function
		 *\param[in]	p_pfnFunction	Receives the function
		 *\param[in]	p_name		The function name
		 *\return		\p true if the function was correctly retrieved
		 *\~french
		 *\brief		Récupère une fonction
		 *\param[in]	p_pfnFunction	Reçoit la fonction
		 *\param[in]	p_name		Le nom de la fonction
		 *\return		\p true si la fonction a été correctement récupérée
		 */
		template< typename FuncType >
		bool getFunction( FuncType & p_pfnFunction, String const & p_name )throw()
		{
			p_pfnFunction = reinterpret_cast< FuncType >( doGetFunction( p_name ) );
			return p_pfnFunction != nullptr;
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
			return m_pLibrary != nullptr;
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
		CU_API void doClose()throw();
		CU_API void * doGetFunction( String const & p_name )throw();

	private:
		void * m_pLibrary;
		Path m_pathLibrary;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		03/01/2011
	\~english
	\brief		Typedef on a pointer to a DynamicLibrary
	\~french
	\brief		Typedef d'un pointeur sur une DynamicLibrary
	*/
	DECLARE_SMART_PTR( DynamicLibrary );
}

#endif
