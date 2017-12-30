/* See LICENSE file in root folder */
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

		inline castor3d::RenderWindowSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::RenderWindowSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( RenderTarget, IRenderTarget *, makeGetter( m_internal.get(), &castor3d::RenderWindow::getRenderTarget ), makePutter( m_internal.get(), &castor3d::RenderWindow::setRenderTarget ) );

		COM_PROPERTY_GET( Size, ISize *, makeGetter( m_internal.get(), &castor3d::RenderWindow::getSize ) );
		COM_PROPERTY_GET( Name, BSTR, makeGetter( m_internal.get(), &castor3d::RenderWindow::getName ) );

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
		castor::real doTransformX( int x );
		castor::real doTransformY( int y );
		int doTransformX( castor::real x );
		int doTransformY( castor::real y );

	private:
		castor3d::RenderWindowSPtr m_internal;
		int m_oldX;
		int m_oldY;
		int m_newX;
		int m_newY;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderWindow ), CRenderWindow );

	DECLARE_VARIABLE_PTR_GETTER( RenderWindow, castor3d, RenderWindow );
	DECLARE_VARIABLE_PTR_PUTTER( RenderWindow, castor3d, RenderWindow );
}

#endif
