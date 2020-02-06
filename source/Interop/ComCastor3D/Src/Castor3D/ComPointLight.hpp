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
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComLightCategory.hpp"

#include <PointLight.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CPointLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CPointLight accessible depuis COM.
	*/
	class ATL_NO_VTABLE CPointLight
		:	COM_ATL_OBJECT( PointLight )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPointLight();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPointLight();

		inline Castor3D::PointLightSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::PointLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Colour, IVector3D *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetColour ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetColour ) );
		COM_PROPERTY( AmbientIntensity, float, make_getter( m_internal.get(), &Castor3D::LightCategory::GetAmbientIntensity ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetAmbientIntensity ) );
		COM_PROPERTY( DiffuseIntensity, float, make_getter( m_internal.get(), &Castor3D::LightCategory::GetDiffuseIntensity ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, make_getter( m_internal.get(), &Castor3D::LightCategory::GetSpecularIntensity ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetSpecularIntensity ) );
		COM_PROPERTY( Position, IVector3D *, make_getter( m_internal.get(), &Castor3D::PointLight::GetPosition ), make_putter( m_internal.get(), &Castor3D::PointLight::SetPosition ) );
		COM_PROPERTY( Attenuation, IVector3D *, make_getter( m_internal.get(), &Castor3D::PointLight::GetAttenuation ), make_putter( m_internal.get(), &Castor3D::PointLight::SetAttenuation ) );

	private:
		Castor3D::PointLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PointLight ), CPointLight );

	DECLARE_VARIABLE_PTR_GETTER( PointLight, Castor3D, PointLight );
	DECLARE_VARIABLE_PTR_PUTTER( PointLight, Castor3D, PointLight );
}

#endif
