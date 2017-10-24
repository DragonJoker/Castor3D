/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_DIRECTIONAL_LIGHT_H__
#define __COMC3D_COM_DIRECTIONAL_LIGHT_H__

#include "ComLightCategory.hpp"

#include <Scene/Light/DirectionalLight.hpp>

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
		CDirectionalLight();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CDirectionalLight();

		inline castor3d::DirectionalLightSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::DirectionalLightSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Colour, IVector3D *, make_getter( m_internal.get(), &castor3d::LightCategory::getColour ), make_putter( m_internal.get(), &castor3d::LightCategory::setColour ) );
		COM_PROPERTY( AmbientIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getAmbientIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setAmbientIntensity ) );
		COM_PROPERTY( DiffuseIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getDiffuseIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setDiffuseIntensity ) );
		COM_PROPERTY( SpecularIntensity, float, make_getter( m_internal.get(), &castor3d::LightCategory::getSpecularIntensity ), make_putter( m_internal.get(), &castor3d::LightCategory::setSpecularIntensity ) );

	private:
		castor3d::DirectionalLightSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DirectionalLight ), CDirectionalLight );

	DECLARE_VARIABLE_PTR_GETTER( DirectionalLight, castor3d, DirectionalLight );
	DECLARE_VARIABLE_PTR_PUTTER( DirectionalLight, castor3d, DirectionalLight );
}

#endif
