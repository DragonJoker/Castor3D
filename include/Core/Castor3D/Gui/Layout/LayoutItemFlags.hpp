/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayoutItemFlags_H___
#define ___C3D_LayoutItemFlags_H___

#include "Castor3D/Gui/GuiModule.hpp"

namespace castor3d
{
	struct LayoutItemFlags
	{
		LayoutItemFlags & align( HAlign v )noexcept
		{
			m_hAlign = v;
			return *this;
		}

		LayoutItemFlags & align( VAlign v )noexcept
		{
			m_vAlign = v;
			return *this;
		}

		LayoutItemFlags & centerHorizontal()noexcept
		{
			return align( HAlign::eCenter );
		}

		LayoutItemFlags & centerVertical()noexcept
		{
			return align( VAlign::eCenter );
		}

		LayoutItemFlags & top()noexcept
		{
			return align( VAlign::eTop );
		}

		LayoutItemFlags & bottom()noexcept
		{
			return align( VAlign::eBottom );
		}

		LayoutItemFlags & left()noexcept
		{
			return align( HAlign::eLeft );
		}

		LayoutItemFlags & right()noexcept
		{
			return align( HAlign::eRight );
		}

		LayoutItemFlags & stretch( bool v )noexcept
		{
			m_expand = v;
			return *this;
		}

		LayoutItemFlags & reserveSpaceIfHidden( bool v )noexcept
		{
			m_reserveSpaceIfHidden = v;
			return *this;
		}

		auto hAlign()const noexcept
		{
			return m_hAlign;
		}

		auto vAlign()const noexcept
		{
			return m_vAlign;
		}

		auto expand()const noexcept
		{
			return m_expand;
		}

		auto reserveSpaceIfHidden()const noexcept
		{
			return m_reserveSpaceIfHidden;
		}

	private:
		HAlign m_hAlign{};
		VAlign m_vAlign{};
		bool m_expand{};
		bool m_reserveSpaceIfHidden{};
		castor::Point4ui m_padding{};
	};
}

#endif
