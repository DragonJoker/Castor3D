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
#ifndef __COMC3D_COM_SPOT_OLIGHT_H__
#define __COMC3D_COM_SPOT_OLIGHT_H__

#include "ComLightCategory.hpp"
#include "ComVector3D.hpp"

#include <SpotLight.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSpotLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CSpotLight accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSpotLight
		:	COM_ATL_OBJECT( SpotLight )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSpotLight();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSpotLight();

		inline Castor3D::SpotLightSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::SpotLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Ambient, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetAmbient ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetAmbient ) );
		COM_PROPERTY( Diffuse, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetDiffuse ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetDiffuse ) );
		COM_PROPERTY( Specular, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetSpecular ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetSpecular ) );
		COM_PROPERTY( Position, IVector3D *, make_getter( m_internal.get(), &Castor3D::SpotLight::GetPosition ), make_putter( m_internal.get(), &Castor3D::SpotLight::SetPosition ) );
		COM_PROPERTY( Attenuation, IVector3D *, make_getter( m_internal.get(), &Castor3D::SpotLight::GetAttenuation ), make_putter( m_internal.get(), &Castor3D::SpotLight::SetAttenuation ) );
		COM_PROPERTY( Exponent, float, make_getter( m_internal.get(), &Castor3D::SpotLight::GetExponent ), make_putter( m_internal.get(), &Castor3D::SpotLight::SetExponent ) );
		COM_PROPERTY( CutOff, float, make_getter( m_internal.get(), &Castor3D::SpotLight::GetCutOff ), make_putter( m_internal.get(), &Castor3D::SpotLight::SetCutOff ) );

	private:
		Castor3D::SpotLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpotLight ), CSpotLight );

	DECLARE_VARIABLE_PTR_GETTER( SpotLight, Castor3D, SpotLight );
	DECLARE_VARIABLE_PTR_PUTTER( SpotLight, Castor3D, SpotLight );
}

#endif
