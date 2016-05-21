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

		inline Castor3D::LightSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::LightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY_GET( Name, BSTR, make_getter( m_internal.get(), &Castor3D::MovableObject::GetName ) );
		COM_PROPERTY_GET( Type, eMOVABLE_TYPE, make_getter( m_internal.get(), &Castor3D::MovableObject::GetType ) );
		COM_PROPERTY_GET( Scene, IScene *, make_getter( m_internal.get(), &Castor3D::MovableObject::GetScene ) );
		COM_PROPERTY_GET( LightType, eLIGHT_TYPE, make_getter( m_internal.get(), &Castor3D::Light::GetLightType ) );
		COM_PROPERTY_GET( DirectionalLight, IDirectionalLight *, make_getter( m_internal.get(), &Castor3D::Light::GetDirectionalLight ) );
		COM_PROPERTY_GET( PointLight, IPointLight *, make_getter( m_internal.get(), &Castor3D::Light::GetPointLight ) );
		COM_PROPERTY_GET( SpotLight, ISpotLight *, make_getter( m_internal.get(), &Castor3D::Light::GetSpotLight ) );

		STDMETHOD( AttachTo )( /* [in] */ ISceneNode * val );
		STDMETHOD( Detach )();
	private:
		Castor3D::LightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Light ), CLight );

	DECLARE_VARIABLE_PTR_GETTER( Light, Castor3D, Light );
	DECLARE_VARIABLE_PTR_PUTTER( Light, Castor3D, Light );
}

#endif
