/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RGBACOLOUR_H__
#define __COMC3D_COM_RGBACOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Colour.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, RgbaColour );
	/*!
	\~english
	\brief		This class defines a CColour object accessible from COM.
	\~french
	\brief		Cette classe définit un CColour accessible depuis COM
	*/
	class CRgbaColour
		: public CComAtlObject< RgbaColour, castor::RgbaColour >
	{
	public:
		/**
		*\~english
		*\brief		Implicit conversion operator, to castor::Point4f.
		*\~french
		*\brief		Opérateur de conversion implicite vers castor::Point4f.
		*/
		inline operator castor::Point4f()const
		{
			return toBGRAFloat( m_internal );
		}

		COMEX_PROPERTY_IDX( R, castor::RgbaComponent::eRed, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( G, castor::RgbaComponent::eGreen, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( B, castor::RgbaComponent::eBlue, FLOAT, &m_internal, &Internal::get, &Internal::get );
		COMEX_PROPERTY_IDX( A, castor::RgbaComponent::eAlpha, FLOAT, &m_internal, &Internal::get, &Internal::get );
	};
	//!\~english	Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french		Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RgbaColour ), CRgbaColour );
}

#endif
