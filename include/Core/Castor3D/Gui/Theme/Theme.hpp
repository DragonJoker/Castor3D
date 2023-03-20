/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Theme_H___
#define ___C3D_Theme_H___

#include "StyleButton.hpp"
#include "StyleComboBox.hpp"
#include "StyleEdit.hpp"
#include "StyleExpandablePanel.hpp"
#include "StyleFrame.hpp"
#include "StyleListBox.hpp"
#include "StylePanel.hpp"
#include "StyleSlider.hpp"
#include "StyleStatic.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace castor3d
{
	class Theme
		: public StylesHolder
	{
	public:
		Theme( castor::String const & name
			, Scene * scene
			, Engine & engine )
			: StylesHolder{ name, engine }
			, m_scene{ scene }
		{
		}

		Theme( castor::String const & name
			, Engine & engine )
			: Theme{ name, nullptr, engine }
		{
		}

		auto & getName()const noexcept
		{
			return getHolderName();
		}

		bool hasScene()const noexcept
		{
			return m_scene != nullptr;
		}

		Scene & getScene()const noexcept
		{
			CU_Require( hasScene() );
			return *m_scene;
		}

	private:
		Scene * m_scene{};
	};
}

#endif
