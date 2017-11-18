/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RENDER_TARGET_H__
#define __COMC3D_COM_RENDER_TARGET_H__

#include "ComSize.hpp"
#include "ComCamera.hpp"
#include "ComScene.hpp"

#include <Render/RenderTarget.hpp>

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
	class ATL_NO_VTABLE CRenderTarget
		:	COM_ATL_OBJECT( RenderTarget )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRenderTarget();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRenderTarget();

		inline castor3d::RenderTargetSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::RenderTargetSPtr value )
		{
			m_internal = value;
		}

		COM_PROPERTY( Camera, ICamera *, makeGetter( m_internal.get(), &castor3d::RenderTarget::getCamera ), makePutter( m_internal.get(), &castor3d::RenderTarget::setCamera ) );
		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, makeGetter( m_internal.get(), &castor3d::RenderTarget::getViewportType ), makePutter( m_internal.get(), &castor3d::RenderTarget::setViewportType ) );
		COM_PROPERTY( Scene, IScene *, makeGetter( m_internal.get(), &castor3d::RenderTarget::getScene ), makePutter( m_internal.get(), &castor3d::RenderTarget::setScene ) );
		COM_PROPERTY( PixelFormat, ePIXEL_FORMAT, makeGetter( m_internal.get(), &castor3d::RenderTarget::getPixelFormat ), makePutter( m_internal.get(), &castor3d::RenderTarget::setPixelFormat ) );

		COM_PROPERTY_GET( Size, ISize *, makeGetter( m_internal.get(), &castor3d::RenderTarget::getSize ) );

		STDMETHOD( Initialise )( /* [in] */ unsigned int index );
		STDMETHOD( Cleanup )();

	private:
		castor3d::RenderTargetSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderTarget ), CRenderTarget );

	DECLARE_VARIABLE_PTR_GETTER( RenderTarget, castor3d, RenderTarget );
	DECLARE_VARIABLE_PTR_PUTTER( RenderTarget, castor3d, RenderTarget );
}

#endif
