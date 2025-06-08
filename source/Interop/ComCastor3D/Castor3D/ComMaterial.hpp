/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MATERIAL_H__
#define __COMC3D_COM_MATERIAL_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Material, C3DMaterial );
	/*!
	\~english
	\brief		This class defines a CMaterial object accessible from COM.
	\~french
	\brief		Cette classe définit un CMaterial accessible depuis COM.
	*/
	class CMaterial
		: public CComAtlObjectT< Material, C3DMaterial >
	{
	public:
		COM_PROPERTY_GET( PassCount, UINT, c3dMaterial_getPassCount );

		COM_DESTROY( CMaterial, c3dMaterial_delete );
		STDMETHOD( CreatePass )( /*[out, retval]*/ IPass ** pVal )override;
		STDMETHOD( GetPass )( /*[in]*/ UINT val, /*[out, retval]*/ IPass ** pVal )override;
		STDMETHOD( RemovePass )( /*[in]*/ IPass * val )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Material ), CMaterial );
}

#endif
