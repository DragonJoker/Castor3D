/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_CAMERA_H__
#define __COMC3D_COM_CAMERA_H__

#include "ComAtlObject.hpp"
#include "ComScene.hpp"

#include <Scene/Camera.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CCamera object accessible from COM.
	\~french
	\brief		Cette classe définit un CCamera accessible depuis COM.
	*/
	class ATL_NO_VTABLE CCamera
		:	COM_ATL_OBJECT( Camera )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CCamera();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CCamera();

		inline castor3d::CameraSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::CameraSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, make_getter( m_internal.get(), &castor3d::Camera::getViewportType ), make_putter( m_internal.get(), &castor3d::Camera::setViewportType ) );

		COM_PROPERTY_GET( Width, unsigned int, make_getter( m_internal.get(), &castor3d::Camera::getWidth ) );
		COM_PROPERTY_GET( Height, unsigned int, make_getter( m_internal.get(), &castor3d::Camera::getHeight ) );
		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &castor3d::MovableObject::getName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &castor3d::MovableObject::getType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &castor3d::MovableObject::getScene ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
		STDMETHOD( Resize )( /* [in] */ unsigned int width, /* [in] */ unsigned int height );

	private:
		castor3d::CameraSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Camera ), CCamera );

	DECLARE_VARIABLE_PTR_GETTER( Camera, castor3d, Camera );
	DECLARE_VARIABLE_PTR_PUTTER( Camera, castor3d, Camera );
}

#endif
