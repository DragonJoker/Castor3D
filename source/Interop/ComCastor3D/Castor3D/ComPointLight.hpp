/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_POINT_LIGHT_H__
#define __COMC3D_COM_POINT_LIGHT_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComShadow.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( PointLight, C3DPointLight );
	/*!
	\~english
	\brief		This class defines a CPointLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CPointLight accessible depuis COM.
	*/
	class CPointLight
		: public CComAtlObjectT< PointLight, C3DPointLight >
	{
	public:
		COM_PROPERTY_SPTR( Colour, IVector3D, c3dPointLight_getColour, c3dPointLight_setColour );
		COM_PROPERTY_GET_MPTR( Shadows, IShadow, c3dPointLight_getShadows );
		COM_PROPERTY( AttenuationRange, FLOAT, c3dPointLight_getAttenuationRange, c3dPointLight_setAttenuationRange );
		COM_PROPERTY( Intensity, FLOAT, c3dPointLight_getIntensity, c3dPointLight_setIntensity );

		COM_DESTROY( CPointLight, c3dPointLight_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( PointLight ), CPointLight );
}

#endif
