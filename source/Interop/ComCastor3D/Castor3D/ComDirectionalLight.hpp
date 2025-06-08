/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_DIRECTIONAL_LIGHT_H__
#define __COMC3D_COM_DIRECTIONAL_LIGHT_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/Castor3D/ComShadow.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( DirectionalLight, C3DDirectionalLight );
	/*!
	\~english
	\brief		This class defines a CDirectionalLight object accessible from COM.
	\~french
	\brief		Cette classe définit un CDirectionalLight accessible depuis COM.
	*/
	class CDirectionalLight
		: public CComAtlObjectT< DirectionalLight, C3DDirectionalLight >
	{
	public:
		COM_PROPERTY_SPTR( Colour, IVector3D, c3dDirectionalLight_getColour, c3dDirectionalLight_setColour );
		COM_PROPERTY_GET_MPTR( Shadows, IShadow, c3dDirectionalLight_getShadows );
		COM_PROPERTY( Illumination, float, c3dDirectionalLight_getIllumination, c3dDirectionalLight_setIllumination );

		COM_DESTROY( CDirectionalLight, c3dDirectionalLight_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( DirectionalLight ), CDirectionalLight );
}

#endif
