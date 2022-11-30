/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RGBCOLOUR_H__
#define __COMC3D_COM_RGBCOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Colour.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, RgbColour );
	/*!
	\~english
	\brief		This class defines a CColour object accessible from COM.
	\~french
	\brief		Cette classe définit un CColour accessible depuis COM
	*/
	class CRgbColour
		: public CComAtlObject< RgbColour, castor::RgbColour >
	{
	public:
		/**
		*\~english
		*\brief		Implicit conversion operator, to castor::Point4f.
		*\~french
		*\brief		Opérateur de conversion implicite vers castor::Point4f.
		*/
		inline operator castor::Point3f()const
		{
			return toBGRFloat( m_internal );
		}

		COMEX_PROPERTY_IDX( R, castor::RgbComponent::eRed, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( G, castor::RgbComponent::eGreen, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( B, castor::RgbComponent::eBlue, FLOAT, &m_internal, &Internal::get, &Internal::get );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RgbColour ), CRgbColour );
}

#endif
