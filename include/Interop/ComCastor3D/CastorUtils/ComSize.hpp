/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SIZE_H__
#define __COMC3D_COM_SIZE_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, Size );
	/*!
	\~english
	\brief		This class defines a CSize object accessible from COM.
	\~french
	\brief		Cette classe définit un CSize accessible depuis COM.
	*/
	class CSize
		: public CComAtlObject< Size, castor::Size >
	{
	public:
		COMEX_PROPERTY( Width, UINT, &m_internal, &Internal::getWidth, &Internal::getWidth );
		COMEX_PROPERTY( Height, UINT, &m_internal, &Internal::getHeight, &Internal::getHeight );

		STDMETHOD( Set )( /* [in] */ UINT x, /* [in] */ UINT y );
		STDMETHOD( Grow )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Size ), CSize );
}

#endif
