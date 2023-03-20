/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Theme_H___
#define ___C3D_Theme_H___

#include "StyleButton.hpp"
#include "StyleComboBox.hpp"
#include "StyleEdit.hpp"
#include "StyleExpandablePanel.hpp"
#include "StyleListBox.hpp"
#include "StylePanel.hpp"
#include "StyleSlider.hpp"
#include "StyleStatic.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace castor3d
{
	template< typename StyleT >
	StyleT * StylesHolder::createControlStyle( castor::String name
		, Scene * scene
		, std::map< castor::String, castor::UniquePtr< StyleT > > & controls )
	{
		auto fullName{ name };

		if ( !m_holderName.empty() )
		{
			fullName = m_holderName + cuT( "/" ) + fullName;
		}

		auto it = controls.find( name );

		if ( it != controls.end() )
		{
			log::warn << "StylesHolder: Duplicate style [" + name + "]" << std::endl;
		}
		else
		{
			if constexpr ( std::is_same_v< StyleT, PanelStyle > )
			{
				it = controls.emplace( name
					, castor::makeUnique< StyleT >( fullName
						, scene
						, m_engine ) ).first;
			}
			else
			{
				it = controls.emplace( name
					, castor::makeUnique< StyleT >( fullName
						, scene
						, m_engine
						, getDefaultFont()->getName() ) ).first;
			}
		}

		return it->second.get();
	}

	template< typename StyleT >
	StyleT * StylesHolder::getControlStyle( castor::String name
		, std::map< castor::String, castor::UniquePtr< StyleT > > const & controls )const
	{
		auto it = controls.find( name );

		if ( it != controls.end() )
		{
			return it->second.get();
		}

		StyleT * style{};
		auto panelIt = std::find_if( m_panelStyles.begin()
			, m_panelStyles.end()
			, [&name, &style]( auto const & lookup )
			{
				if ( name.find( lookup.first + "/" ) == 0u )
				{
					style = lookup.second->template getStyle< StyleT >( name.substr( lookup.first.size() + 1u ) );
				}

				return style != nullptr;
			} );

		if ( panelIt == m_panelStyles.end() )
		{
			auto expandPanelIt = std::find_if( m_expandablePanelStyles.begin()
				, m_expandablePanelStyles.end()
				, [&name, &style]( auto const & lookup )
				{
					auto header = lookup.first + "/Header/";
					auto panel = lookup.first + "/Content/";

					if ( name.find( header ) == 0u )
					{
						style = lookup.second->getHeaderStyle().template getStyle< StyleT >( name.substr( header.size() ) );
					}
					else if ( name.find( panel ) == 0u )
					{
						style = lookup.second->getContentStyle().template getStyle< StyleT >( name.substr( panel.size() ) );
					}

					return style != nullptr;
				} );

			if ( expandPanelIt == m_expandablePanelStyles.end() )
			{
				style = nullptr;
			}
		}

		if ( style == nullptr )
		{
			CU_SrcException( "StylesHolder", "Couldn't find style" );
		}

		return style;
	}

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
