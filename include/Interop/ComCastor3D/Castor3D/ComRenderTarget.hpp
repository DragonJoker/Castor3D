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

		COMEX_PROPERTY( ViewportType, eVIEWPORT_TYPE, m_internal, &castor3d::RenderTarget::getViewportType, &castor3d::RenderTarget::setViewportType );
		COMEX_PROPERTY( Camera, ICamera *, m_internal, &castor3d::RenderTarget::getCamera, setCamera );
		COMEX_PROPERTY( Scene, IScene *, m_internal, &castor3d::RenderTarget::getScene, setScene );

		COMEX_PROPERTY_GET( PixelFormat, ePIXEL_FORMAT, m_internal, &castor3d::RenderTarget::getPixelFormat );
		COMEX_PROPERTY_GET( Size, ISize *, m_internal, &castor3d::RenderTarget::getSize );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderTarget ), CRenderTarget );
}

#endif
