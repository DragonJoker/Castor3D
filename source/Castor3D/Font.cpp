#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Font.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"

using namespace Castor3D;

VertexBufferPtr TextFont::VertexBuffer;
IndexBufferPtr TextFont::IndexBuffer;

BufferElementDeclaration TextFont::DeclarationElements[] =
{
	{ 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_2FLOATS },
	{ 0, eELEMENT_USAGE_DIFFUSE,	eELEMENT_TYPE_COLOUR },
	{ 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS }
};

//*************************************************************************************************

TextFont :: TextFont( String const & p_strFontName)
{
	Collection<Material, String> l_mtlCollection;
	m_pMaterial = l_mtlCollection.GetElement( p_strFontName);
	if ( ! m_pMaterial)
	{
		m_pMaterial = MaterialPtr( new Material( p_strFontName, 1));
	}

	Collection<Font, String> l_collection;
	m_pFont = l_collection.GetElement( p_strFontName);

	if (m_pFont)
	{
		Collection<Image, String> l_imgCollection;
		m_pImage = l_imgCollection.GetElement( cuT( "Img") + p_strFontName);
		if ( ! m_pImage)
		{
			m_pImage = ImagePtr( new Image( cuT( "Img") + p_strFontName));
			l_imgCollection.AddElement( cuT( "Img") + p_strFontName, m_pImage);
		}
		m_pImage->Initialise( Point<unsigned int, 2>( 256 * m_pFont->GetMaxWidth(), 256 * m_pFont->GetMaxHeight()));
		std::vector <unsigned char> l_imgBuffer;

		m_pTexture = m_pMaterial->GetPass( 0)->AddTextureUnit();
		m_pTexture->SetTexture( eTEXTURE_TYPE_2D, m_pImage);
		// Paramètrage du rendu
		m_pMaterial->GetPass( 0)->SetSrcBlendFactor( Pass::eSRC_BLEND_SRC_ALPHA);
		m_pMaterial->GetPass( 0)->SetDstBlendFactor( Pass::eDST_BLEND_ONE_MINUS_SRC_ALPHA);
		m_pTexture->SetRgbMode( TextureUnit::eRGB_BLEND_FUNC_MODULATE);
		m_pTexture->SetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_0, TextureUnit::eBLEND_SOURCE_TEXTURE);
		m_pTexture->SetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_1, TextureUnit::eBLEND_SOURCE_DIFFUSE);
		m_pTexture->SetAlpMode( TextureUnit::eALPHA_BLEND_FUNC_MODULATE);
		m_pTexture->SetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_0, TextureUnit::eBLEND_SOURCE_TEXTURE);
		m_pTexture->SetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_1, TextureUnit::eBLEND_SOURCE_DIFFUSE);
	}
}

TextFont :: ~TextFont()
{
	m_pFont.reset();
}

void TextFont :: Initialise()
{
	CASTOR_TRACK;
	if ( ! VertexBuffer)
	{
		TextFont::VertexBuffer = RenderSystem::CreateVertexBuffer( DeclarationElements);
		TextFont::VertexBuffer->reserve( 256 << 2, 0);
		TextFont::IndexBuffer = RenderSystem::CreateIndexBuffer();
		TextFont::IndexBuffer->reserve( 3 << 9, 0);

		for (size_t i = 0; i < 256 ; ++i)
		{
			TextFont::IndexBuffer->push_back( 0 + (i << 2));
			TextFont::IndexBuffer->push_back( 1 + (i << 2));
			TextFont::IndexBuffer->push_back( 2 + (i << 2));
			TextFont::IndexBuffer->push_back( 2 + (i << 2));
			TextFont::IndexBuffer->push_back( 1 + (i << 2));
			TextFont::IndexBuffer->push_back( 3 + (i << 2));
		}

		TextFont::VertexBuffer->Initialise( eBUFFER_MODE_DYNAMIC);
		TextFont::IndexBuffer->Initialise( eBUFFER_MODE_STATIC);
	}
}

void TextFont :: DisplayText( String const & p_strText, const Point2r & p_ptPosition, const Point2r & p_ptSize, Colour const & p_clrColour)
{
	CASTOR_TRACK;
	if ( ! p_strText.empty() && m_pTexture && m_pFont)
	{
		Initialise();
		float         l_fRatio   	= p_strText.size() * 16.0f / m_pTexture->GetWidth();
		float         l_fOff     	= 0.5f / m_pTexture->GetWidth();
		float         l_fTexUnit 	= 1.0f / 16.0f;
		float         x       		= static_cast<float>( p_ptPosition[0]);
		float         y       		= static_cast<float>( p_ptPosition[1]);

		TVertex * l_pVertices = static_cast<TVertex *>( TextFont::VertexBuffer->Lock( 0, 0, eLOCK_FLAG_WRITE_ONLY));

		if (l_pVertices)
		{
			int l_iNbChars = 0;

			for (size_t i = 0 ; i < p_strText.size() && l_iNbChars < 256 ; ++i)
			{
				unsigned char c = p_strText.char_str()[i];
				// Gestion des caractères spéciaux
				switch (c)
				{
				// Saut de ligne
				case '\n' :
					x = static_cast<float>( p_ptPosition[0]);
					y += m_pFont->GetHeight() * l_fRatio;
					break;
				// Retour au début de ligne
				case '\r' :
					x = static_cast<float>( p_ptPosition[0]);
					break;
				// Tabulation horizontale
				case '\t' :
					x += 4 * m_pFont->GetHeight() * l_fRatio;
					break;

				case '\v' :
					y += 4 * m_pFont->GetHeight() * l_fRatio;
					continue;
				// Espace
				case ' ' :
					x += m_pFont->GetHeight() * l_fRatio;
					continue;
				}
				// On va pointer sur les vertices du quad courant
				TVertex * V = l_pVertices + l_iNbChars * 4;
				// Position
				V[0].Position[0] = x;						V[0].Position[1] = y;
				V[1].Position[0] = x + p_strText.size();	V[1].Position[1] = y;
				V[2].Position[0] = x;               		V[2].Position[1] = y + p_strText.size();
				V[3].Position[0] = x + p_strText.size();	V[3].Position[1] = y + p_strText.size();
				// Couleur
				V[0].Diffuse = p_clrColour.ARGB();
				V[1].Diffuse = p_clrColour.ARGB();
				V[2].Diffuse = p_clrColour.ARGB();
				V[3].Diffuse = p_clrColour.ARGB();
				// Coordonnées de texture
				V[0].TexCoords[0] = l_fTexUnit * ((c % 16) + 0) + l_fOff;V[0].TexCoords[1] = l_fTexUnit * ((c / 16) + 0) + l_fOff;
				V[1].TexCoords[0] = l_fTexUnit * ((c % 16) + 1) - l_fOff;V[1].TexCoords[1] = l_fTexUnit * ((c / 16) + 0) + l_fOff;
				V[2].TexCoords[0] = l_fTexUnit * ((c % 16) + 0) + l_fOff;V[2].TexCoords[1] = l_fTexUnit * ((c / 16) + 1) - l_fOff;
				V[3].TexCoords[0] = l_fTexUnit * ((c % 16) + 1) - l_fOff;V[3].TexCoords[1] = l_fTexUnit * ((c / 16) + 1) - l_fOff;
				// Incrémentation de la position et du nombre de caractères
				x += m_pFont->GetHeight() * l_fRatio;
				++l_iNbChars;
    		}

    		TextFont::VertexBuffer->Unlock();
			// Affichage du texte
			m_pMaterial->Render( ePRIMITIVE_TYPE_TRIANGLES);
			TextFont::VertexBuffer->Activate();
			TextFont::IndexBuffer->Activate();
			RenderSystem::GetSingletonPtr()->DrawIndexedPrimitives( ePRIMITIVE_TYPE_TRIANGLES, 0, 0, 0, l_iNbChars * 2);
			m_pMaterial->EndRender();
		}
	}
}

//*************************************************************************************************
