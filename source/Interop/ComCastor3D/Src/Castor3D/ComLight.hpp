/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHT_H__
#define __COMC3D_COM_LIGHT_H__

#include "ComDirectionalLight.hpp"
#include "ComPointLight.hpp"
#include "ComSpotLight.hpp"

#include <Scene/Light/Light.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CLight accessible depuis COM.
	*/
	class ATL_NO_VTABLE CLight
		:	COM_ATL_OBJECT( Light )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CLight();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CLight();

		inline castor3d::LightSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::LightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Name, BSTR, makeGetter( m_internal.get(), &castor3d::MovableObject::getName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, makeGetter( m_internal.get(), &castor3d::MovableObject::getType ) );
		COM_PROPERTY_GET( Scene, IScene *, makeGetter( m_internal.get(), &castor3d::MovableObject::getScene ) );
		COM_PROPERTY_GET( LightType, eLIGHT_TYPE, makeGetter( m_internal.get(), &castor3d::Light::getLightType ) );
		COM_PROPERTY_GET( DirectionalLight, IDirectionalLight *, makeGetter( m_internal.get(), &castor3d::Light::getDirectionalLight ) );
		COM_PROPERTY_GET( PointLight, IPointLight *, makeGetter( m_internal.get(), &castor3d::Light::getPointLight ) );
		COM_PROPERTY_GET( SpotLight, ISpotLight *, makeGetter( m_internal.get(), &castor3d::Light::getSpotLight ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
	private:
		castor3d::LightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Light ), CLight );

	DECLARE_VARIABLE_PTR_GETTER( Light, castor3d, Light );
	DECLARE_VARIABLE_PTR_PUTTER( Light, castor3d, Light );
}

#endif
