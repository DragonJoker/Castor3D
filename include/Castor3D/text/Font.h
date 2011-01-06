#ifndef ___C3D_Font___
#define ___C3D_Font___

#include "../Prerequisites.h"

namespace Castor3D
{
	class FontTexture
	{
	private:
		FontPtr m_pFont;
		Plane * m_pPlanes;

	public:
		FontTexture( const String & p_strName, const String & p_strPath, size_t p_uiHeight);
		~FontTexture();

		void DrawText( const String & p_strText, const Point2r & p_ptPosition, const Point2r & p_ptSize);
	};
}

#endif