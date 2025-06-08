/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SIZE_H__
#define __COMC3D_COM_SIZE_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( Size, C3DSize );
	/*!
	\~english
	\brief		This class defines a CSize object accessible from COM.
	\~french
	\brief		Cette classe définit un CSize accessible depuis COM.
	*/
	class CSize
		: public CComAtlObjectT< Size, C3DSize >
	{
	public:
		COM_PROPERTY_MBR( Width, UINT, &m_internal.width );
		COM_PROPERTY_MBR( Height, UINT, &m_internal.height );

		STDMETHOD( Set )( /*[in]*/ UINT x, /*[in]*/ UINT y )override;
		STDMETHOD( Grow )( /*[in]*/ INT x, /*[in]*/ INT y )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Size ), CSize );
}

#endif
