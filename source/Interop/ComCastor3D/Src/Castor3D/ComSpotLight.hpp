/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_SPOT_OLIGHT_H__
#define __COMC3D_COM_SPOT_OLIGHT_H__

#include "ComLightCategory.hpp"
#include "ComAngle.hpp"

#include <Scene/Light/SpotLight.hpp>

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

		inline castor3d::SpotLightSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::SpotLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Colour, IVector3D *, make_getter( m_internal.get(), &castor3d::LightCategory::getColour ), make_putter( m_internal.get(), &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( AmbientIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getAmbientIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setAmbientIntensity ) );
		COM_PROPERTY( DiffuseIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getDiffuseIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getSpecularIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setSpecularIntensity ) );
		COM_PROPERTY( Attenuation, IVector3D *, make_getter( m_internal.get(), &castor3d::SpotLight::getAttenuation ), make_putter( m_internal.get(), &castor3d::SpotLight::setAttenuation ) );
		COM_PROPERTY( Exponent, float, make_getter( m_internal.get(), &castor3d::SpotLight::getExponent ), make_putter( m_internal.get(), &castor3d::SpotLight::setExponent ) );
		COM_PROPERTY( CutOff, IAngle *, make_getter( m_internal.get(), &castor3d::SpotLight::getCutOff ), make_putter( m_internal.get(), &castor3d::SpotLight::setCutOff ) );

	private:
		castor3d::SpotLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpotLight ), CSpotLight );

	DECLARE_VARIABLE_PTR_GETTER( SpotLight, castor3d, SpotLight );
	DECLARE_VARIABLE_PTR_PUTTER( SpotLight, castor3d, SpotLight );
}

#endif
