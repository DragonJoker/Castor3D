/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_HDRRGBCOLOUR_H__
#define __COMC3D_COM_HDRRGBCOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( HdrRgbColour, C3DHdrRgbColour );
	/*!
	\~english
	\brief		This class defines a HdrColour object accessible from COM.
	\~french
	\brief		Cette classe définit un HdrColour accessible depuis COM
	*/
	class CHdrRgbColour
		: public CComAtlObjectT< HdrRgbColour, C3DHdrRgbColour >
	{
	public:
		COM_PROPERTY_MBR( R, FLOAT, &m_internal.r );
		COM_PROPERTY_MBR( G, FLOAT, &m_internal.g );
		COM_PROPERTY_MBR( B, FLOAT, &m_internal.b );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( HdrRgbColour ), CHdrRgbColour );
}

#endif
