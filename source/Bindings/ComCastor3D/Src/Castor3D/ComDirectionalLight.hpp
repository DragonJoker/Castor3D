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
#ifndef __COMC3D_COM_DIRECTIONAL_LIGHT_H__
#define __COMC3D_COM_DIRECTIONAL_LIGHT_H__

#include "ComLightCategory.hpp"
#include "ComVector3D.hpp"

#include <DirectionalLight.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CDirectionalLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CDirectionalLight accessible depuis COM.
	*/
	class ATL_NO_VTABLE CDirectionalLight
		:	COM_ATL_OBJECT( DirectionalLight )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CDirectionalLight();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CDirectionalLight();

		inline Castor3D::DirectionalLightSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::DirectionalLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Ambient, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetAmbient ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetAmbient ) );
		COM_PROPERTY( Diffuse, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetDiffuse ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetDiffuse ) );
		COM_PROPERTY( Specular, IColour *, make_getter( m_internal.get(), &Castor3D::LightCategory::GetSpecular ), make_putter( m_internal.get(), &Castor3D::LightCategory::SetSpecular ) );
		COM_PROPERTY( Direction, IVector3D *, make_getter( m_internal.get(), &Castor3D::DirectionalLight::GetDirection ), make_putter( m_internal.get(), &Castor3D::DirectionalLight::SetDirection ) );

	private:
		Castor3D::DirectionalLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DirectionalLight ), CDirectionalLight );

	DECLARE_VARIABLE_PTR_GETTER( DirectionalLight, Castor3D, DirectionalLight );
	DECLARE_VARIABLE_PTR_PUTTER( DirectionalLight, Castor3D, DirectionalLight );
}

#endif
