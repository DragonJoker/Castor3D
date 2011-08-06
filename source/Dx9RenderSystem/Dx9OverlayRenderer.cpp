#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9OverlayRenderer.hpp"

using namespace Castor3D;

void Dx9OverlayRenderer :: DrawPanel()
{
	MaterialPtr l_pMaterial = ((PanelOverlay *)m_target)->GetMaterial();

	if (l_pMaterial)
	{
		l_pMaterial->Render2D();

		// TODO : rendre les overlays

		l_pMaterial->EndRender();
	}
}

void Dx9OverlayRenderer :: DrawBorderPanel()
{
}

void Dx9OverlayRenderer :: DrawText()
{
	TextOverlayPtr l_pText = static_pointer_cast<TextOverlay>( m_target->GetOverlayCategory());
	l_pText->GetFont()->DisplayText( l_pText->GetCaption(), l_pText->GetPosition(), l_pText->GetSize(), Colour::FullAlphaWhite);
}
