/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GLYPH_H__
#define __COMC3D_COM_GLYPH_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"

#include <CastorUtils/Graphics/Glyph.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor, Glyph );
	/*!
	\~english
	\brief		This class defines a CGlyph object accessible from COM.
	\~french
	\brief		Cette classe définit un CGlyph accessible depuis COM.
	*/
	class CGlyph
		: public CComAtlObject< Glyph, castor::Glyph >
	{
	public:
		COM_PROPERTY_GET( Size, ISize *, makeGetter( m_internal, &castor::Glyph::getSize ) );
		COM_PROPERTY_GET( Bearing, IPosition *, makeGetter( m_internal, &castor::Glyph::getBearing ) );
		COM_PROPERTY_GET( Advance, INT, makeGetter( m_internal, &castor::Glyph::getAdvance ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Glyph ), CGlyph );
}

#endif
