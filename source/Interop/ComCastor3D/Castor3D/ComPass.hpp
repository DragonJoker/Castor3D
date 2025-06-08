/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComHdrRgbColour.hpp"
#include "ComCastor3D/CastorUtils/ComRgbColour.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Pass, C3DPass );
	/*!
	\~english
	\brief		This class defines a CPass object accessible from COM.
	\~french
	\brief		Cette classe définit un CPass accessible depuis COM.
	*/
	class CPass
		: public CComAtlObjectT< Pass, C3DPass >
	{
	public:
		COM_PROPERTY_EX( TwoSided, boolean, bool, c3dPass_getTwoSided, c3dPass_setTwoSided );
		COM_PROPERTY( Opacity, FLOAT, c3dPass_getOpacity, c3dPass_setOpacity );
		COM_PROPERTY_SPTR( EmissiveColour, IRgbColour, c3dPass_getEmissiveColour, c3dPass_setEmissiveColour );
		COM_PROPERTY( EmissiveFactor, FLOAT, c3dPass_getEmissiveFactor, c3dPass_setEmissiveFactor );
		COM_PROPERTY( RefractionRatio, FLOAT, c3dPass_getRefractionRatio, c3dPass_setRefractionRatio );
		COM_PROPERTY_EX( AlphaFunc, eCOMPARISON_FUNC, C3D_COMPARISON_FUNC, c3dPass_getAlphaFunc, c3dPass_setAlphaFunc );
		COM_PROPERTY_EX( BlendAlphaFunc, eCOMPARISON_FUNC, C3D_COMPARISON_FUNC, c3dPass_getBlendAlphaFunc, c3dPass_setBlendAlphaFunc );
		COM_PROPERTY( AlphaRefValue, FLOAT, c3dPass_getAlphaRefValue, c3dPass_setAlphaRefValue );
		COM_PROPERTY_SPTR( Diffuse, IRgbColour, c3dPass_getDiffuse, c3dPass_setDiffuse );
		COM_PROPERTY_SPTR( Albedo, IHdrRgbColour, c3dPass_getAlbedo, c3dPass_setAlbedo );
		COM_PROPERTY_SPTR( Specular, IRgbColour, c3dPass_getSpecular, c3dPass_setSpecular );
		COM_PROPERTY( SpecularFactor, FLOAT, c3dPass_getSpecularFactor, c3dPass_setSpecularFactor );
		COM_PROPERTY( Metalness, FLOAT, c3dPass_getMetalness, c3dPass_setMetalness );
		COM_PROPERTY( Shininess, FLOAT, c3dPass_getShininess, c3dPass_setShininess );
		COM_PROPERTY( Roughness, FLOAT, c3dPass_getRoughness, c3dPass_setRoughness );
		COM_PROPERTY( Glossiness, FLOAT, c3dPass_getGlossiness, c3dPass_setGlossiness );

		COM_DESTROY( CPass, c3dPass_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );
}

#endif
