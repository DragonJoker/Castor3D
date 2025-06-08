/* See LICENSE file in root folder */
#ifndef __COMC3D_ComSkyboxBackground_H__
#define __COMC3D_ComSkyboxBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Skybox, C3DSkybox );
	/*!
	\~english
	\brief		This class defines a CSkybox object accessible from COM.
	\~french
	\brief		Cette classe définit un CSkybox accessible depuis COM.
	*/
	class CSkybox
		: public CComAtlObjectT< Skybox, C3DSkybox >
	{
	public:
		COM_PROPERTY_PUT_EX( LeftImage, BSTR, C3DString, c3dSkybox_setLeftImage );
		COM_PROPERTY_PUT_EX( RightImage, BSTR, C3DString, c3dSkybox_setRightImage );
		COM_PROPERTY_PUT_EX( TopImage, BSTR, C3DString, c3dSkybox_setTopImage );
		COM_PROPERTY_PUT_EX( BottomImage, BSTR, C3DString, c3dSkybox_setBottomImage );
		COM_PROPERTY_PUT_EX( FrontImage, BSTR, C3DString, c3dSkybox_setFrontImage );
		COM_PROPERTY_PUT_EX( BackImage, BSTR, C3DString, c3dSkybox_setBackImage );
		COM_PROPERTY_PUT_EX( CrossImage, BSTR, C3DString, c3dSkybox_setCrossImage );

		COM_DESTROY( CSkybox, c3dSkybox_delete );
		STDMETHOD( SetEquirectangularImage )( /*[in]*/ BSTR filePath, /*[in]*/ UINT size )override;
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( Skybox ), CSkybox );
}

#endif
