/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_HDRRGBCOLOUR_H__
#define __COMC3D_COM_HDRRGBCOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Colour.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, HdrRgbColour );
	/*!
	\~english
	\brief		This class defines a HdrColour object accessible from COM.
	\~french
	\brief		Cette classe définit un HdrColour accessible depuis COM
	*/
	class CHdrRgbColour
		: public CComAtlObject< HdrRgbColour, castor::HdrRgbColour >
	{
	public:
		COMEX_PROPERTY_IDX( R, castor::RgbComponent::eRed, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( G, castor::RgbComponent::eGreen, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( B, castor::RgbComponent::eBlue, FLOAT, &m_internal, &Internal::get, &Internal::get );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( HdrRgbColour ), CHdrRgbColour );
}

#endif
