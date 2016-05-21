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
#ifndef __COMC3D_COM_RENDER_WINDOW_H__
#define __COMC3D_COM_RENDER_WINDOW_H__

#include "ComRenderTarget.hpp"

#include <Render/RenderWindow.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRenderWindow
		:	COM_ATL_OBJECT( RenderWindow )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRenderWindow();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRenderWindow();

		inline Castor3D::RenderWindowSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::RenderWindowSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( RenderTarget, IRenderTarget *, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetRenderTarget ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetRenderTarget ) );

		COM_PROPERTY_GET( Size, ISize *, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetSize ) );
		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetName ) );

		STDMETHOD( Initialise )( /* [in] */ ISize * size, /* [in] */ LPVOID val, /* [out, retval] */ VARIANT_BOOL * pVal );
		STDMETHOD( Cleanup )();
		STDMETHOD( Resize )( /* [in] */ ISize * size );
		STDMETHOD( OnMouseMove )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseLButtonDown )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseLButtonUp )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseMButtonDown )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseMButtonUp )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseRButtonDown )( /* [in] */ IPosition * pos );
		STDMETHOD( OnMouseRButtonUp )( /* [in] */ IPosition * pos );

	private:
		Castor::real DoTransformX( int x );
		Castor::real DoTransformY( int y );
		int DoTransformX( Castor::real x );
		int DoTransformY( Castor::real y );

	private:
		Castor3D::RenderWindowSPtr m_internal;
		int m_oldX;
		int m_oldY;
		int m_newX;
		int m_newY;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderWindow ), CRenderWindow );

	DECLARE_VARIABLE_PTR_GETTER( RenderWindow, Castor3D, RenderWindow );
	DECLARE_VARIABLE_PTR_PUTTER( RenderWindow, Castor3D, RenderWindow );
}

#endif
