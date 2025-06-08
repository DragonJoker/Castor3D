/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RENDER_WINDOW_H__
#define __COMC3D_COM_RENDER_WINDOW_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( RenderWindow, C3DRenderWindow );
	/*!
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class CRenderWindow
		: public CComAtlObjectT< RenderWindow, C3DRenderWindow >
	{
	public:
		COM_PROPERTY_GET_EX( Name, BSTR, C3DString, c3dRenderWindow_getName );

		COM_DESTROY( CRenderWindow, c3dRenderWindow_delete );
		STDMETHOD( Initialise )( /*[in]*/ IRenderTarget * target )override;
		STDMETHOD( Cleanup )()override;
		STDMETHOD( Resize )( /*[in]*/ ISize * size )override;
		STDMETHOD( OnMouseMove )( /*[in]*/ IPosition * pos, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseLButtonDown )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseLButtonUp )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseMButtonDown )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseMButtonUp )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseRButtonDown )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnMouseRButtonUp )( /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnKeyboardKeyDown )( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnKeyboardKeyUp )( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ boolean isCtrlDown, /*[in]*/ boolean isAltDown, /*[in]*/ boolean isShiftDown, /*[out, retval]*/ boolean * pVal )override;
		STDMETHOD( OnKeyboardChar )( /*[in]*/ eKEYBOARD_KEY key, /*[in]*/ BSTR c, /*[out, retval]*/ boolean * pVal )override;

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
