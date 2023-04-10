/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_FONT_H__
#define __COMC3D_COM_FONT_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorCom
{
	class CFont;
	template<>
	struct ComITypeTraitsT< ICastorFont >
	{
		static constexpr bool hasIType = true;
		using Type = castor::Font;
	};
	template<>
	struct ComTypeTraitsT< castor::Font >
	{
		static constexpr bool hasIType = true;
		static constexpr bool hasType = true;
		static constexpr bool hasInternalType = true;

		using IType = ICastorFont;
		using CType = CFont;

		static inline const CLSID clsid = CLSID_Font;
		static inline const CLSID iid = IID_ICastorFont;
		static inline const UINT rid = IDR_Font;

		using Internal = castor::Font;
		using InternalMbr = castor::FontResPtr;
		using GetInternal = castor::FontResPtr;
		using SetInternal = castor::FontResPtr;
	};
	/*!
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class CFont
		: public CComAtlObject< Font, castor::Font >
	{
	public:
		COMEX_PROPERTY_GET( Height, UINT, m_internal, &castor::Font::getHeight );
		COMEX_PROPERTY_GET( MaxHeight, UINT, m_internal, &castor::Font::getMaxHeight );
		COMEX_PROPERTY_GET( MaxWidth, UINT, m_internal, &castor::Font::getMaxWidth );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR path, /* [in] */ BSTR name, /* [in] */ UINT height );
		STDMETHOD( GetGlyph )( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph );

	private:
		std::map< WORD, IGlyph * > m_glyphs;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Font ), CFont );
}

#endif
