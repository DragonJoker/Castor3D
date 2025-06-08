/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GLYPH_H__
#define __COMC3D_COM_GLYPH_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/CastorUtils/ComVector2D.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( Glyph, C3DGlyph );
	/*!
	\~english
	\brief		This class defines a CGlyph object accessible from COM.
	\~french
	\brief		Cette classe définit un CGlyph accessible depuis COM.
	*/
	class CGlyph
		: public CComAtlObjectT< Glyph, C3DGlyph >
	{
	public:
		COM_PROPERTY_GET_SPTR( Size, IVector2D, c3dGlyph_getSize );
		COM_PROPERTY_GET_SPTR( Bearing, IVector2D, c3dGlyph_getBearing );
		COM_PROPERTY_GET( Advance, float, c3dGlyph_getAdvance );
		COM_PROPERTY_GET_SPTR( BitmapSize, ISize, c3dGlyph_getBitmapSize );

		COM_DESTROY( CGlyph, c3dGlyph_delete );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Glyph ), CGlyph );
}

#endif
