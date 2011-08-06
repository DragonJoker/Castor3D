#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlOverlayRenderer.hpp"
#include "GlRenderSystem/GlBuffer.hpp"

using namespace Castor3D;

void GlOverlayRenderer :: DrawPanel()
{
	CASTOR_TRACK;
	MaterialPtr l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if (l_pMaterial)
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
		CheckGlError( glEnd(), cuT( "GlOverlayRenderer :: DrawPanel"));
*/
		l_pMaterial->EndRender();
	}
}

void GlOverlayRenderer :: DrawBorderPanel()
{
	CASTOR_TRACK;
}

void GlOverlayRenderer :: DrawText()
{
	CASTOR_TRACK;
	TextOverlayPtr l_pText = static_pointer_cast<TextOverlay>( m_target->GetOverlayCategory());
	l_pText->GetFont()->DisplayText( l_pText->GetCaption(), l_pText->GetPosition(), l_pText->GetSize(), Colour::FullAlphaWhite);
}
