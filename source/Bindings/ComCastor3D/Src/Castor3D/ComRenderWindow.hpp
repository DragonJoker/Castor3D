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

#include "ComSize.hpp"
#include "ComCamera.hpp"
#include "ComScene.hpp"

#include <RenderWindow.hpp>

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
		COMC3D_API CRenderWindow();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CRenderWindow();

		inline Castor3D::RenderWindowSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::RenderWindowSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( SamplesCount, unsigned int, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetSamplesCount ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetSamplesCount ) );
		COM_PROPERTY( Camera, ICamera *, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetCamera ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetCamera ) );
		COM_PROPERTY( PrimitiveType, eTOPOLOGY, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetPrimitiveType ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetPrimitiveType ) );
		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetViewportType ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetViewportType ) );
		COM_PROPERTY( Scene, IScene *, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetScene ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetScene ) );
		COM_PROPERTY( PixelFormat, ePIXEL_FORMAT, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetPixelFormat ), make_putter( m_internal.get(), &Castor3D::RenderWindow::SetPixelFormat ) );

		COM_PROPERTY_GET( Size, ISize *, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetSize ) );
		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::RenderWindow::GetName ) );

		STDMETHOD( Initialise )( /* [in] */ LPVOID val, /* [out, retval] */ VARIANT_BOOL * pVal );
		STDMETHOD( Cleanup )();
		STDMETHOD( Resize )( /* [in] */ ISize * size );
	private:
		Castor3D::RenderWindowSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderWindow ), CRenderWindow )

	template< typename Class >
	struct VariableGetter< Class, Castor3D::RenderWindowSPtr >
	{
		typedef Castor3D::RenderWindowSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IRenderWindow ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CRenderWindow::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CRenderWindow * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IRenderWindow,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class >
	struct VariablePutter< Class, Castor3D::RenderWindowSPtr >
	{
		typedef void ( Class::*Function )( Castor3D::RenderWindowSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( IRenderWindow * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CRenderWindow * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IRenderWindow,						// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
}

#endif
