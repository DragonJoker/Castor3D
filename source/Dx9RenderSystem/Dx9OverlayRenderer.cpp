#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9OverlayRenderer.h"

using namespace Castor3D;

void Dx9OverlayRenderer :: DrawPanel()
{
	MaterialPtr l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if (l_pMaterial != NULL)
	{
		l_pMaterial->Render2D();
/*
		glBegin( GL_QUADS);
		{
			glVertex2r( m_target->GetPosition()[0], m_target->GetPosition()[1]);
			glTexCoord2r( 0.0, 0.0);
			glVertex2r( m_target->GetPosition()[0], m_target->GetPosition()[1] + m_target->GetSize()[1]);
			glTexCoord2r( 0.0, 1.0);
			glVertex2r( m_target->GetPosition()[0] + m_target->GetSize()[0], m_target->GetPosition()[1] + m_target->GetSize()[1]);
			glTexCoord2r( 1.0, 1.0);
			glVertex2r( m_target->GetPosition()[0] + m_target->GetSize()[0], m_target->GetPosition()[1]);
			glTexCoord2r( 1.0, 0.0);
		}
		glEnd();
		CheckDxError( CU_T( "Dx9OverlayRenderer :: DrawPanel"));
*/
		l_pMaterial->EndRender();
	}
}

void Dx9OverlayRenderer :: DrawBorderPanel()
{
}

void Dx9OverlayRenderer :: DrawText()
{
	TextOverlay * l_pText = static_cast<TextOverlay *>( m_target);
	l_pText->GetFont()->DisplayText( l_pText->GetCaption(), l_pText->GetPosition(), l_pText->GetSize(), Colour::FullAlphaWhite);
}
