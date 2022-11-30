/* See LICENSE file in root folder */
#ifndef __COMC3D_ComImageBackground_H__
#define __COMC3D_ComImageBackground_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <Castor3D/Scene/Background/Image.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, ImageBackground );
	/*!
	\~english
	\brief		This class defines a CImageBackground object accessible from COM.
	\~french
	\brief		Cette classe définit un CImageBackground accessible depuis COM.
	*/
	class CImageBackground
		: public CComAtlObject< ImageBackground, castor3d::ImageBackground >
	{
	public:
		COM_PROPERTY_GET( Type
			, BSTR
			, makeGetter( m_internal, &castor3d::SceneBackground::getType ) );
		STDMETHOD( put_Image )( /* [in] */ BSTR filePath );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object.
	//\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet.
	OBJECT_ENTRY_AUTO( __uuidof( ImageBackground ), CImageBackground );
}

#endif
