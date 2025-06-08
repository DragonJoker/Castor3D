/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_FONT_H__
#define __COMC3D_COM_FONT_H__

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComVector2D.hpp"

namespace CastorCom
{
	class CFont;
	template<>
	struct ComITypeTraitsT< ICastorFont >
	{
		static constexpr bool hasIType = true;
		using Type = C3DFont;
	};
	template<>
	struct ComTypeTraitsT< C3DFont >
	{
		static constexpr bool hasIType = true;
		static constexpr bool hasType = true;
		static constexpr bool hasInternalType = true;

		using IType = ICastorFont;
		using CType = CFont;

		static inline const CLSID clsid = CLSID_Font;
		static inline const CLSID iid = IID_ICastorFont;
		static inline const UINT rid = IDR_Font;

		using Internal = C3DFont;
		using InternalPtr = C3DFont *;
		using InternalMbr = InternalPtr;
		using GetConstInternal = C3DFont const *;
		using GetInternal = C3DFont *&;
		using SetInternal = C3DFont *;
	};
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CFont
		: public CComAtlObjectT< Font, C3DFont >
	{
	public:
		COM_PROPERTY_GET( Height, UINT, c3dFont_getHeight );
		COM_PROPERTY_GET( MaxGlyphHeight, UINT, c3dFont_getMaxGlyphHeight );
		COM_PROPERTY_GET( MaxGlyphWidth, UINT, c3dFont_getMaxGlyphWidth );
		COM_PROPERTY_GET( MaxImageHeight, UINT, c3dFont_getMaxImageHeight );
		COM_PROPERTY_GET( MaxImageWidth, UINT, c3dFont_getMaxImageWidth );
		COM_PROPERTY_GET_SPTR( MaxBearing, IVector2D, c3dFont_getMaxBearing );

		COM_DESTROY( CFont, c3dFont_delete );
		STDMETHOD( GetGlyph )( /*[in]*/ WORD glyph, /*[out, retval]*/ IGlyph ** pRet )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Font ), CFont );
}

#endif
