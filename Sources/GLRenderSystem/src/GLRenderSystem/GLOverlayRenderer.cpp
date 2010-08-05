#include "PrecompiledHeader.h"

#include "GLOverlayRenderer.h"

using namespace Castor3D;

void GLOverlayRenderer :: DrawPanel()
{
	Material * l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if (l_pMaterial != NULL)
	{
		l_pMaterial->Apply( DTQuads);

		glBegin( GL_QUADS);
		{
			glVertex2f( m_target->GetPosition().x, m_target->GetPosition().y);
			glTexCoord2f( 0.0, 0.0);
			glVertex2f( m_target->GetPosition().x, m_target->GetPosition().y + m_target->GetSize().y);
			glTexCoord2f( 0.0, 1.0);
			glVertex2f( m_target->GetPosition().x + m_target->GetSize().x, m_target->GetPosition().y + m_target->GetSize().y);
			glTexCoord2f( 1.0, 1.0);
			glVertex2f( m_target->GetPosition().x + m_target->GetSize().x, m_target->GetPosition().y);
			glTexCoord2f( 1.0, 0.0);
		}
		glEnd();

		l_pMaterial->Remove();
	}
}

void GLOverlayRenderer :: DrawBorderPanel()
{
}

void GLOverlayRenderer :: DrawText()
{
}