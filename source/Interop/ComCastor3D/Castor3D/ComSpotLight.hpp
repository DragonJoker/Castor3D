/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SPOT_LIGHT_H__
#define __COMC3D_COM_SPOT_LIGHT_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComShadow.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( SpotLight, C3DSpotLight );
	/*!
	\~english
	\brief		This class defines a CSpotLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CSpotLight accessible depuis COM.
	*/
	class CSpotLight
		: public CComAtlObjectT< SpotLight, C3DSpotLight >
	{
	public:
		COM_PROPERTY_SPTR( Colour, IVector3D, c3dSpotLight_getColour, c3dSpotLight_setColour );
		COM_PROPERTY_GET_MPTR( Shadows, IShadow, c3dSpotLight_getShadows );
		COM_PROPERTY( AttenuationRange, FLOAT, c3dSpotLight_getAttenuationRange, c3dSpotLight_setAttenuationRange );
		COM_PROPERTY( Intensity, FLOAT, c3dSpotLight_getIntensity, c3dSpotLight_setIntensity );
		COM_PROPERTY( Exponent, FLOAT, c3dSpotLight_getExponent, c3dSpotLight_setExponent );
		COM_PROPERTY( InnerCutOff, FLOAT, c3dSpotLight_getInnerCutOff, c3dSpotLight_setInnerCutOff );
		COM_PROPERTY( OuterCutOff, FLOAT, c3dSpotLight_getOuterCutOff, c3dSpotLight_setOuterCutOff );

		COM_DESTROY( CSpotLight, c3dSpotLight_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( SpotLight ), CSpotLight );
}

#endif
