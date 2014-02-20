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
#ifndef ___C3D_Font___
#define ___C3D_Font___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Graphic font representation
	\remark		Holds a font adn the buffers used to draw text using this font
	\~french
	\brief		Représentation dessinable d'une font
	\remark		Gère la police et le tampon utilisé pour la dessiner
	*/
	class C3D_API TextFont
	{
	private:
		struct TextVertex
		{
			real		Position[2];
			real		TexCoords[2];
			uint32_t	Diffuse;
		};
		typedef std::pair< uint32_t, uint32_t > UI32UI32Pair;
		DECLARE_MAP( wchar_t, UI32UI32Pair, OffsetChar );

		Castor::FontWPtr m_wpFont;
		Engine * m_pEngine;
		SamplerWPtr m_wpSampler;
		//!\~english	The blend state	\~french	L'objet de gestion des états de mélange
		BlendStateSPtr m_pBlendState;
		//!\~english	The texture that will receive the glyphs	\~french	La texture qui recevra les glyphes
		DynamicTextureSPtr m_pTexture;
		//!\~english	The GeometryBuffers used to render one character	\~french	Le GeometryBuffers utilisé pour rendre un caractère
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english	Buffer elements declaration	\~french	Déclaration des éléments d'un vertex
		Castor3D::BufferDeclarationSPtr m_pDeclaration;
		//!\~english	Vertex array (quad definition)	\~french	Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 4 > m_arrayVertex;
		//!	4 * ([3(vertex position) + 2(texture coordinates)] * sizeof( real ) + sizeof( uint32_t ))
		TextVertex m_pBuffer[4];
		//!\~english	Glyphs X offset	\~french	Décalage X des glyphes
		OffsetCharMap m_mapGlyphOffsets;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_strFontName	The font name
		 *\param[in]	p_pEngine		The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_strFontName	Le nom de la police
		 *\param[in]	p_pEngine		Le moteur
		 */
		TextFont( Castor::String const & p_strFontName, Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextFont();
		/**
		 *\~english
		 *\brief		Initialises members
		 *\~french
		 *\brief		Iniitialise les membres
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the font members
		 *\~french
		 *\brief		Nettoie les membres
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Displays a given text at given position with given colour
		 *\param[in]	p_strText		The text
		 *\param[in]	p_pProgram		The program used to render the text
		 *\param[in]	p_clrColour		The colour
		 *\~french
		 *\brief		Affiche un texte à la position donnée avec la couleur donnée
		 *\param[in]	p_strText		Le texte
		 *\param[in]	p_pProgram		Le programme utilisé pour rendre le texte
		 *\param[in]	p_clrColour		La couleur
		 */
		void DisplayText( Castor::String const & p_strText,Castor::Colour const & p_clrColour, ShaderProgramBaseSPtr p_pProgram );
		/**
		 *\~english
		 *\brief		Retrieves font name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de la police
		 *\return		Le nom
		 */
		Castor::String const & GetFontName()const;

	private:
		inline Castor::FontSPtr DoGetFont()const { return m_wpFont.lock(); }
	};
}

#pragma warning( pop )

#endif
