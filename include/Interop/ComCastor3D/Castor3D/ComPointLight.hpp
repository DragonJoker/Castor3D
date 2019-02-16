/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComCastor3D/Castor3D/ComLightCategory.hpp"

#include <Castor3D/Scene/Light/PointLight.hpp>

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

		inline castor3d::PointLightSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::PointLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Colour, IVector3D *, makeGetter( m_internal.get(), &castor3d::LightCategory::getColour ), makePutter( m_internal.get(), &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( DiffuseIntensity, float, makeGetter( m_internal.get(), &castor3d::LightCategory::getDiffuseIntensity ), makePutter( m_internal.get(), &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, makeGetter( m_internal.get(), &castor3d::LightCategory::getSpecularIntensity ), makePutter( m_internal.get(), &castor3d::LightCategory::setSpecularIntensity ) );
		COM_PROPERTY( Attenuation, IVector3D *, makeGetter( m_internal.get(), &castor3d::PointLight::getAttenuation ), makePutter( m_internal.get(), &castor3d::PointLight::setAttenuation ) );

	private:
		castor3d::PointLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PointLight ), CPointLight );

	DECLARE_VARIABLE_PTR_GETTER( PointLight, castor3d, PointLight );
	DECLARE_VARIABLE_PTR_PUTTER( PointLight, castor3d, PointLight );
}

#endif
