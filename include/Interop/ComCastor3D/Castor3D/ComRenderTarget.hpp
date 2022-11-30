/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RENDER_TARGET_H__
#define __COMC3D_COM_RENDER_TARGET_H__

#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/Castor3D/ComCamera.hpp"
#include "ComCastor3D/Castor3D/ComScene.hpp"

#include <Castor3D/Render/RenderTarget.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, RenderTarget );
	/*!
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRenderTarget
		: public CComAtlObject< RenderTarget, castor3d::RenderTarget >
	{
	public:
		static void setCamera( castor3d::RenderTarget * target, castor3d::Camera * camera )
		{
			target->setCamera( *camera );
		}

		static void setScene( castor3d::RenderTarget * target, castor3d::Scene * scene )
		{
			target->setScene( *scene );
		}

		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, makeGetter( m_internal, &castor3d::RenderTarget::getViewportType ), makePutter( m_internal, &castor3d::RenderTarget::setViewportType ) );
		COM_PROPERTY( Camera, ICamera *, makeGetter( m_internal, &castor3d::RenderTarget::getCamera ), makePutter( m_internal, setCamera ) );
		COM_PROPERTY( Scene, IScene *, makeGetter( m_internal, &castor3d::RenderTarget::getScene ), makePutter( m_internal, setScene ) );

		COM_PROPERTY_GET( PixelFormat, ePIXEL_FORMAT, makeGetter( m_internal, &castor3d::RenderTarget::getPixelFormat ) );
		COM_PROPERTY_GET( Size, ISize *, makeGetter( m_internal, &castor3d::RenderTarget::getSize ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderTarget ), CRenderTarget );
}

#endif
