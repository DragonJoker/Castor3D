/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_WindowHandle_HPP___
#define ___Renderer_WindowHandle_HPP___

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Implémentation d'un handle de fenêtre, dépendant de l'OS.
	*/
	class IWindowHandle
	{
	protected:
		/**
		*\brief
		*	Constructeur
		*/
		IWindowHandle() = default;

	public:
		/**
		*\brief
		*	Opérateur de conversion en booléen.
		*/
		virtual operator bool() = 0;
	};
	/**
	*\brief
	*	Implémentation d'un handle de fenêtre, indépendant de l'OS.
	*/
	class WindowHandle
	{
	public:
		WindowHandle( WindowHandle const & ) = delete;
		WindowHandle & operator=( WindowHandle const & ) = delete;
		WindowHandle( WindowHandle && rhs ) = default;
		WindowHandle & operator=( WindowHandle && rhs ) = default;
		/**
		*\brief
		*	Constructeur
		*\param[in] handle
		*	Le handle alloué.
		*/
		explicit WindowHandle( IWindowHandlePtr handle );
		/**
		*\brief
		*	Destructeur
		*/
		~WindowHandle() = default;
		/**
		*\brief
		*	opérateur de conversion en bool.
		*/
		operator bool();
		/**
		*\brief
		*	Récupère le handle de la fenêtre, casté dans le format voulu (HWND, GLXDrawable, ...).
		*\return
		*	Le handle de la fenêtre.
		*/
		template< class T >
		inline T const & getInternal()const
		{
			return static_cast< T const & >( *m_handle );
		}

	private:
		IWindowHandlePtr m_handle;
	};
}

#endif
