/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ScrollableStyle_H___
#define ___C3D_ScrollableStyle_H___

#include "StyleScrollable.hpp"

namespace castor3d
{
	class ScrollableStyle
	{
	public:
		void setVerticalStyle( ScrollBarStyle & v )
		{
			m_verticalStyle = &v;
		}

		void setHorizontalStyle( ScrollBarStyle & v )
		{
			m_horizontalStyle = &v;
		}

		bool hasVerticalStyle()const noexcept
		{
			return m_verticalStyle != nullptr;
		}

		bool hasHorizontalStyle()const noexcept
		{
			return m_horizontalStyle != nullptr;
		}

		ScrollBarStyle & getVerticalStyle()const noexcept
		{
			CU_Require( hasVerticalStyle() );
			return *m_verticalStyle;
		}

		ScrollBarStyle & getHorizontalStyle()const noexcept
		{
			CU_Require( hasHorizontalStyle() );
			return *m_horizontalStyle;
		}

	private:
		ScrollBarStyle * m_verticalStyle{};
		ScrollBarStyle * m_horizontalStyle{};
	};
}

#endif
