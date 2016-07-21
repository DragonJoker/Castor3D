/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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

		inline void SetInternal( Castor::FontSPtr p_font )
		{
			m_font = p_font;
		}

		inline Castor::FontSPtr GetInternal()const
		{
			return m_font;
		}

		COM_PROPERTY_GET( Height, UINT, make_getter( m_font.get(), &Castor::Font::GetHeight ) );
		COM_PROPERTY_GET( MaxHeight, INT, make_getter( m_font.get(), &Castor::Font::GetMaxHeight ) );
		COM_PROPERTY_GET( MaxWidth, INT, make_getter( m_font.get(), &Castor::Font::GetMaxWidth ) );

		STDMETHOD( LoadFromFile )( /* [in] */ IEngine * engine, /* [in] */ BSTR path, /* [in] */ BSTR name, /* [in] */ UINT height );
		STDMETHOD( GetGlyph )( /* [in] */ WORD glyph, /* [out, retval] */ IGlyph ** pGlyph );

	private:
		Castor::FontSPtr m_font;
		std::map< WORD, IGlyph * > m_glyphs;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Font ), CFont );

	DECLARE_VARIABLE_PTR_GETTER( Font, Castor, Font );
	DECLARE_VARIABLE_PTR_PUTTER( Font, Castor, Font );
}

#endif
