/* See LICENSE file in root folder */
#ifndef __COMC3D_ComLineMapping_H__
#define __COMC3D_ComLineMapping_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( LineMapping, C3DLineMapping );
	/*!
	\~english
	\brief		This class defines a CSubmesh object accessible from COM.
	\~french
	\brief		Cette classe définit un CSubmesh accessible depuis COM.
	*/
	class CLineMapping
		: public CComAtlObjectT< LineMapping, C3DLineMapping >
	{
	public:
		COM_PROPERTY_GET( LinesCount, UINT, c3dLineMapping_getLinesCount );

		COM_DESTROY( CLineMapping, c3dLineMapping_delete );
		STDMETHOD( AddLine )( /*[in]*/ UINT x, /*[in]*/ UINT y )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( LineMapping ), CLineMapping );
}

#endif
