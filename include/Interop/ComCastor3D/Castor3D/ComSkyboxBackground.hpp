/* See LICENSE file in root folder */
#ifndef __COMC3D_ComSkyboxBackground_H__
#define __COMC3D_ComSkyboxBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Background/Skybox.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, SkyboxBackground );
	/*!
	\~english
	\brief		This class defines a CSkyboxBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CSkyboxBackground accessible depuis COM.
	*/
	class CSkyboxBackground
		: public CComAtlObject< SkyboxBackground, castor3d::SkyboxBackground >
	{
	public:
		COM_PROPERTY_GET( Type
			, BSTR
			, makeGetter( m_internal, &castor3d::SceneBackground::getType ) );
		STDMETHOD( put_LeftImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_RightImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_TopImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_BottomImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_FrontImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_BackImage )( /* [in] */ BSTR filePath );
		STDMETHOD( put_CrossImage )( /* [in] */ BSTR filePath );
		STDMETHOD( LoadEquirectangularImage )( /* [in] */ BSTR filePath, /* [in] */ unsigned int size );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( SkyboxBackground ), CSkyboxBackground );
}

#endif
