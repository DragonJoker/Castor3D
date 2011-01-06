#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLOverlayRenderer.h"

using namespace Castor3D;

void GLOverlayRenderer :: DrawPanel()
{
	MaterialPtr l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if ( ! l_pMaterial == NULL)
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
		glEnd();
		CheckGLError( CU_T( "GLOverlayRenderer :: DrawPanel"));

		l_pMaterial->EndRender();
	}
}

void GLOverlayRenderer :: DrawBorderPanel()
{
}

void GLOverlayRenderer :: DrawText()
{
}