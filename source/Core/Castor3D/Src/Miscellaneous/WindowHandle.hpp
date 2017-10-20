/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WINDOW_HANDLE_H___
#define ___C3D_WINDOW_HANDLE_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/01/2012
	\see		WindowHandle.hpp
	\~english
	\brief		Platform dependant window handle implementation
	\~english
	\brief		Implémentation d'un handle de fenêtre, dépendant de l'OS
	*/
	class IWindowHandle
		: public castor::NonCopyable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API IWindowHandle() {}

	public:
		/**
		 *\~english
		 *\brief		bool cast operator
		 *\~french
		 *\brief		opérateur de conversion en bool
		 */
		C3D_API virtual operator bool() = 0;
	};
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/01/2012
	\~english
	\brief Platform independant window handle implementation
	\~english
	\brief Implémentation d'un handle de fenêtre, indépendant de l'OS
	*/
	class WindowHandle
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API WindowHandle();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pHandle	The allocated handle
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pHandle	Le handle alloué
		 */
		C3D_API explicit WindowHandle( IWindowHandleSPtr p_pHandle );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		C3D_API WindowHandle( WindowHandle const & p_copy );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_copy	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 */
		C3D_API WindowHandle( WindowHandle && p_copy );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		C3D_API WindowHandle & operator=( WindowHandle const & p_copy );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_copy	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		C3D_API WindowHandle & operator=( WindowHandle && p_copy );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~WindowHandle();
		/**
		 *\~english
		 *\brief		bool cast operator
		 *\~french
		 *\brief		opérateur de conversion en bool
		 */
		C3D_API operator bool();
		/**
		 *\~english
		 *\brief		Retrieves the window handle, casted in the wanted format (HWND, GLXDrawable, ...)
		 *\return		The window handle
		 *\~french
		 *\brief		Récupère le handle de la fenêtre, casté dans le format voulu (HWND, GLXDrawable, ...)
		 *\return		Le handle de la fenêtre
		 */
		template< class T >
		inline std::shared_ptr< T > getInternal()const
		{
			return std::static_pointer_cast< T >( m_pHandle );
		}

	private:
		IWindowHandleSPtr m_pHandle;
	};
}

#endif
