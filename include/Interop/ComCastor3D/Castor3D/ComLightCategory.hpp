/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LIGHT_CATEGORY_H__
#define __COMC3D_COM_LIGHT_CATEGORY_H__

#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

#include <Castor3D/Scene/Light/LightCategory.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, LightCategory );
	/*!
	\~english
	\brief		This class defines a CLightCategory object accessible from COM.
	\~french
	\brief		Cette classe définit un CLightCategory accessible depuis COM.
	*/
	class CLightCategory
		: public CComAtlObject< LightCategory, castor3d::LightCategory >
	{
	public:
		COMEX_PROPERTY( Colour, IVector3D *, m_internal, &castor3d::LightCategory::getColour, &castor3d::LightCategory::setColour );
		COMEX_PROPERTY( DiffuseIntensity, float, m_internal, &castor3d::LightCategory::getDiffuseIntensity, &castor3d::LightCategory::setDiffuseIntensity );
		COMEX_PROPERTY( SpecularIntensity, float, m_internal, &castor3d::LightCategory::getSpecularIntensity, &castor3d::LightCategory::setSpecularIntensity );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LightCategory ), CLightCategory );
}

#endif
