/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_MATERIAL_H__
#define __COMC3D_COM_MATERIAL_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"

#include <Castor3D/Material/Material.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, Material );
	/*!
	\~english
	\brief		This class defines a CMaterial object accessible from COM.
	\~french
	\brief		Cette classe définit un CMaterial accessible depuis COM.
	*/
	class CMaterial
		: public CComAtlObject< Material, castor3d::Material >
	{
	public:
		COMEX_PROPERTY_GET( PassCount, unsigned int, m_internal, &castor3d::Material::getPassCount );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( CreatePass )( /* [out, retval] */ IPass ** pVal );
		STDMETHOD( GetPass )( /* [in] */ unsigned int val, /* [out, retval] */ IPass ** pVal );
		STDMETHOD( DestroyPass )( /* [in] */ unsigned int val );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Material ), CMaterial );
}

#endif
