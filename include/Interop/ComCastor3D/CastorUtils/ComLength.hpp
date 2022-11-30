/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ANGLE_H__
#define __COMC3D_COM_ANGLE_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Length.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, Length );
	/*!
	\~english
	\brief		This class defines a CLength object accessible from COM.
	\~french
	\brief		Cette classe définit un CLength accessible depuis COM
	*/
	class CLength
		: public CComAtlObject< Length, castor::Length >
	{
	public:
		COMEX_PROPERTY( Kilometres, FLOAT, &m_internal, &Internal::kilometres, &Internal::kilometres );
		COMEX_PROPERTY( Metres, FLOAT, &m_internal, &Internal::metres, &Internal::metres );
		COMEX_PROPERTY( Centimetres, FLOAT, &m_internal, &Internal::centimetres, &Internal::centimetres );
		COMEX_PROPERTY( Millimetres, FLOAT, &m_internal, &Internal::millimetres, &Internal::millimetres );
		COMEX_PROPERTY( Yards, FLOAT, &m_internal, &Internal::yards, &Internal::yards );
		COMEX_PROPERTY( Feet, FLOAT, &m_internal, &Internal::feet, &Internal::feet );
		COMEX_PROPERTY( Inches, FLOAT, &m_internal, &Internal::inches, &Internal::inches );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Length ), CLength );
}

#endif
