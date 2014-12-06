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
#include <Font.hpp>

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
		COMC3D_API CFont();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CFont();

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
	__declspec( selectany ) ATL::_ATL_OBJMAP_CACHE __objCache__CFont = { NULL, 0 };
	const ATL::_ATL_OBJMAP_ENTRY_EX __objMap_CFont = {&CLSID_Font, CFont::UpdateRegistry, CFont::_ClassFactoryCreatorClass::CreateInstance, CFont::_CreatorClass::CreateInstance, &__objCache__CFont, CFont::GetObjectDescription, CFont::GetCategoryMap, CFont::ObjectMain };
	extern "C" __declspec( allocate( "ATL$__m" ) ) __declspec( selectany ) const ATL::_ATL_OBJMAP_ENTRY_EX * const __pobjMap_CFont = &__objMap_CFont;
	__pragma( comment( linker, "/include:___pobjMap_CFont" ) );

	template< typename Class >
	struct VariableGetter< Class, Castor::FontSPtr >
	{
		typedef Castor::FontSPtr( Class::*Function )()const;
		VariableGetter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ICastorFont ** value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					hr = CFont::CreateInstance( value );

					if ( hr == S_OK )
					{
						static_cast< CFont * >( *value )->SetInternal( ( m_instance->*m_function )() );
					}
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IFont,								// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};

	template< typename Class >
	struct VariablePutter< Class, Castor::FontSPtr >
	{
		typedef void ( Class::*Function )( Castor::FontSPtr );
		VariablePutter( Class * instance, Function function )
			:	m_instance( instance )
			,	m_function( function )
		{
		}
		HRESULT operator()( ICastorFont * value )
		{
			HRESULT hr = E_POINTER;

			if ( m_instance )
			{
				if ( value )
				{
					( m_instance->*m_function )( static_cast< CFont * >( value )->GetInternal() );
					hr = S_OK;
				}
			}
			else
			{
				hr = CComError::DispatchError(
						 E_FAIL,								// This represents the error
						 IID_IFont,								// This is the GUID of component throwing error
						 cuT( "NULL instance" ),				// This is generally displayed as the title
						 ERROR_UNINITIALISED_INSTANCE.c_str(),	// This is the description
						 0,										// This is the context in the help file
						 NULL );
			}

			return hr;
		}

	private:
		Class * m_instance;
		Function m_function;
	};
}

#endif
