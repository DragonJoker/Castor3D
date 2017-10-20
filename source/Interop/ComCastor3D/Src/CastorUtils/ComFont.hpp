/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_FONT_H__
#define __COMC3D_COM_FONT_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Font.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CImage object accessible from COM.
	\~french
	\brief		Cette classe définit un CImage accessible depuis COM.
	*/
	class ATL_NO_VTABLE CFont
		:	 public CComAtlObject< Font, &CLSID_Font, CFont, ICastorFont, &IID_ICastorFont, IDR_Font >
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CFont();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CFont();

		inline void setInternal( castor::FontSPtr p_font )
		{
			m_font = p_font;
		}

		inline castor::FontSPtr getInternal()const
		{
			return m_font;
		}

		COM_PROPERTY_GET( Height, UINT, make_getter( m_font.get(), &castor::Font::getHeight ) );
		COM_PROPERTY_GET( MaxHeight, INT, make_getter( m_font.get(), &castor::Font::getMaxHeight ) );
		COM_PROPERTY_GET( MaxWidth, INT, make_getter( m_font.get(), &castor::Font::getMaxWidth ) );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR path, /* [in] */ BSTR name, /* [in] */ UINT height );
		STDMETHOD( getGlyph )( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph );

	private:
		castor::FontSPtr m_font;
		std::map< WORD, IGlyph * > m_glyphs;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Font ), CFont );

	DECLARE_VARIABLE_PTR_GETTER( Font, castor, Font );
	DECLARE_VARIABLE_PTR_PUTTER( Font, castor, Font );
}

#endif
