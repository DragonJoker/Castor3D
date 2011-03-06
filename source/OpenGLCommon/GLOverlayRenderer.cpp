#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlOverlayRenderer.h"
#include "OpenGlCommon/GlBuffer.h"

using namespace Castor3D;

void GlOverlayRenderer :: DrawPanel()
{
	MaterialPtr l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if (l_pMaterial != NULL)
	{
		l_pMaterial->Render2D();

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
		CheckGlError( glEnd(), CU_T( "GlOverlayRenderer :: DrawPanel"));

		l_pMaterial->EndRender();
	}
}

void GlOverlayRenderer :: DrawBorderPanel()
{
}

void GlOverlayRenderer :: DrawText()
{
	TextOverlay * l_pText = static_cast<TextOverlay *>( m_target);
	l_pText->GetFont()->DisplayText( l_pText->GetCaption(), l_pText->GetPosition(), l_pText->GetSize(), Colour::FullAlphaWhite);
}
