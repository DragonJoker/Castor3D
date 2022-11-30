/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_POSITION_H__
#define __COMC3D_COM_POSITION_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Position.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, Position );
	/*!
	\~english
	\brief		This class defines a CPosition object accessible from COM.
	\~french
	\brief		Cette classe définit un CPosition accessible depuis COM
	*/
	class CPosition
		: public CComAtlObject< Position, castor::Position >
	{
	public:
		COMEX_PROPERTY( X, INT, &m_internal, &Internal::x, &Internal::x );
		COMEX_PROPERTY( Y, INT, &m_internal, &Internal::y, &Internal::y );

		STDMETHOD( Set )( /* [in] */ INT x, /* [in] */ INT y );
		STDMETHOD( Offset )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Position ), CPosition );
}

#endif
