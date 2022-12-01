/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RENDER_WINDOW_H__
#define __COMC3D_COM_RENDER_WINDOW_H__

#include "ComCastor3D/Castor3D/ComRenderTarget.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Render/RenderWindow.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_SPTR( castor3d, RenderWindow );
	/*!
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class CRenderWindow
		: public CComAtlObject< RenderWindow, castor3d::RenderWindow >
	{
	public:
		COMEX_PROPERTY_GET( Name, BSTR, m_internal.get(), &castor3d::RenderWindow::getName );

		STDMETHOD( Initialise )( /* [in] */ IRenderTarget * target );
		STDMETHOD( Cleanup )();
		STDMETHOD( Resize )( /* [in] */ ISize * size );
		STDMETHOD( OnMouseMove )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseLButtonDown )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseLButtonUp )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseMButtonDown )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseMButtonUp )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseRButtonDown )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );
		STDMETHOD( OnMouseRButtonUp )( /* [in] */ IPosition * pos, /* [out, retval] */ boolean * pVal );

	private:
		float doTransformX( int x );
		float doTransformY( int y );
		int doTransformX( float x );
		int doTransformY( float y );

	private:
		int m_oldX{};
		int m_oldY{};
		int m_newX{};
		int m_newY{};
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderWindow ), CRenderWindow );
}

#endif
