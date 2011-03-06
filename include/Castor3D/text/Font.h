/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include "../Prerequisites.h"
#include "../render_system/Buffer.h"

namespace Castor3D
{
	class C3D_API TextFont
	{
	private:
		struct TVertex
		{
			float Position[2];
			unsigned long Diffuse;
			float TexCoords[2];
		};
		FontPtr m_pFont;
		MaterialPtr m_pMaterial;
		TextureUnitPtr m_pTexture;
		static VertexBufferPtr VertexBuffer;
		static IndexBufferPtr IndexBuffer;
		static BufferElementDeclaration DeclarationElements[];
		static BufferDeclaration Declaration;

	public:
		TextFont( FontManager * p_pManager, const String & p_strFontName);
		virtual ~TextFont();

		void Initialise();
		void DisplayText( const String & p_strText, const Point2r & p_ptPosition, const Point2r & p_ptSize, const Colour & p_clrColour);
		void AddChar( xchar cChar, size_t p_uiIndex, Point2r & p_ptPosition);

		static VertexBufferPtr	GetVertexBuffer		()		{ return TextFont::VertexBuffer; }
		static IndexBufferPtr	GetIndexBuffer		()		{ return TextFont::IndexBuffer; }
		inline ImagePtr			GetImage			()const { return m_pFont->GetImage(); }
		inline String			GetFontName			()const { return m_pFont->GetName(); }
	};
}

#endif
