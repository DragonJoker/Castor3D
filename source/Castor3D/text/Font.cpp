#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/text/Font.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/Pass.h"

using namespace Castor3D;

VertexBufferPtr TextFont::VertexBuffer;
IndexBufferPtr TextFont::IndexBuffer;

BufferElementDeclaration TextFont::DeclarationElements[] =
{
	{ 0, eUsagePosition,	eType2Floats },
	{ 0, eUsageDiffuse,		eTypeColour },
	{ 0, eUsageTexCoords0,	eType2Floats }
};

//*************************************************************************************************

TextFont :: TextFont( FontManager * p_pManager, const String & p_strFontName)
	:	m_pFont( p_pManager->GetElement( p_strFontName))
	,	m_pMaterial( RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->CreateMaterial( p_strFontName))
{
	if (m_pFont != NULL)
	{
		m_pTexture = m_pMaterial->GetPass( 0)->AddTextureUnit();
		m_pTexture->SetTexture2D( m_pFont->GetImage());
		// Paramètrage du rendu
		m_pMaterial->GetPass( 0)->SetSrcBlendFactor( Pass::eSrcBlendSrcAlpha);
		m_pMaterial->GetPass( 0)->SetDstBlendFactor( Pass::eDstBlendOneMinusSrcAlpha);
		m_pTexture->SetRgbMode( TextureUnit::eRgbModeModulate);
		m_pTexture->SetRgbArgument( TextureUnit::eArgumentTexture, 0);
		m_pTexture->SetRgbArgument( TextureUnit::eArgumentDiffuse, 1);
		m_pTexture->SetAlpMode( TextureUnit::eAlphaModeModulate);
		m_pTexture->SetAlpArgument( TextureUnit::eArgumentTexture, 0);
		m_pTexture->SetAlpArgument( TextureUnit::eArgumentDiffuse, 1);
	}
}

TextFont :: ~TextFont()
{
	m_pFont.reset();
}

void TextFont :: Initialise()
{
	if (VertexBuffer == NULL)
	{
		TextFont::VertexBuffer = RenderSystem::CreateVertexBuffer( DeclarationElements);
		TextFont::VertexBuffer->InitialiseBuffer( 256 << 2, 0);
		TextFont::IndexBuffer = RenderSystem::CreateIndexBuffer();
		TextFont::IndexBuffer->InitialiseBuffer( 3 << 9, 0);

		for (size_t i = 0; i < 256 ; ++i)
		{
			TextFont::IndexBuffer->Add( 0 + (i << 2));
			TextFont::IndexBuffer->Add( 1 + (i << 2));
			TextFont::IndexBuffer->Add( 2 + (i << 2));
			TextFont::IndexBuffer->Add( 2 + (i << 2));
			TextFont::IndexBuffer->Add( 1 + (i << 2));
			TextFont::IndexBuffer->Add( 3 + (i << 2));
		}

		TextFont::VertexBuffer->Initialise( eBufferDynamic);
		TextFont::IndexBuffer->Initialise( eBufferStatic);
	}
}

void TextFont :: DisplayText( const String & p_strText, const Point2r & p_ptPosition, const Point2r & p_ptSize, const Colour & p_clrColour)
{
	if ( ! p_strText.empty() && m_pTexture != NULL && m_pFont != NULL)
	{
		Initialise();
		float         l_fRatio   	= p_strText.size() * 16.0f / m_pTexture->GetWidth();
		float         l_fOff     	= 0.5f / m_pTexture->GetWidth();
		float         l_fTexUnit 	= 1.0f / 16.0f;
		float         x       		= static_cast<float>( p_ptPosition[0]);
		float         y       		= static_cast<float>( p_ptPosition[1]);

		TVertex * l_pVertices = static_cast<TVertex *>( TextFont::VertexBuffer->Lock( 0, 0, eLockWriteOnly));

		if (l_pVertices != NULL)
		{
			int l_iNbChars = 0;

			for (String::const_iterator i = p_strText.begin() ; (i != p_strText.end()) && (l_iNbChars < 256) ; ++i)
			{
				unsigned char c = *i;
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
				// Tabulation verticale      // On va pointer sur les vertices du quad courant
				TVertex * V = l_pVertices + l_iNbChars * 4;
				// Position
				V[0].Position[0] = x;						V[0].Position[1] = y;
				V[1].Position[0] = x + p_strText.size();	V[1].Position[1] = y;
				V[2].Position[0] = x;               		V[2].Position[1] = y + p_strText.size();
				V[3].Position[0] = x + p_strText.size();	V[3].Position[1] = y + p_strText.size();
				// Couleur
				V[0].Diffuse = p_clrColour.ToLong();
				V[1].Diffuse = p_clrColour.ToLong();
				V[2].Diffuse = p_clrColour.ToLong();
				V[3].Diffuse = p_clrColour.ToLong();
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
			m_pMaterial->Render( eTriangles);
			TextFont::VertexBuffer->Activate();
			TextFont::IndexBuffer->Activate();
			RenderSystem::GetSingletonPtr()->DrawIndexedPrimitives( eTriangles, 0, 0, 0, l_iNbChars * 2);
			m_pMaterial->EndRender();
		}
	}
}

//*************************************************************************************************
