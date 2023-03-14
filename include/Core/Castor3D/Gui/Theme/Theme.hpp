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
	class Theme
		: public castor::Named
	{
	public:
		Theme( castor::String const & name
			, Engine & engine )
			: castor::Named{ name }
			, m_engine{ engine }
			, m_defaultFont{ engine.getFontCache().begin()->second }
		{
		}
		/**
		*\brief
		*	Retrieves the default font used by controls
		*\return
		*	The font
		*/
		castor::FontRes getDefaultFont()const
		{
			return m_defaultFont.lock();
		}
		/**
		*\brief
		*	sets the default font used by controls
		*\param[in] font
		*	The font
		*/
		void setDefaultFont( castor::FontResPtr font )
		{
			m_defaultFont = font;
		}

		ButtonStyleRPtr createButtonStyle( castor::String const & name )
		{
			m_buttonStyles[name] = castor::makeUnique< ButtonStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_buttonStyles[name].get();
		}

		ComboBoxStyleRPtr createComboBoxStyle( castor::String const & name )
		{
			m_comboBoxStyles[name] = castor::makeUnique< ComboBoxStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_comboBoxStyles[name].get();
		}

		EditStyleRPtr createEditStyle( castor::String const & name )
		{
			m_editStyles[name] = castor::makeUnique< EditStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_editStyles[name].get();
		}

		ListBoxStyleRPtr createListBoxStyle( castor::String const & name )
		{
			m_listBoxStyles[name] = castor::makeUnique< ListBoxStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_listBoxStyles[name].get();
		}

		SliderStyleRPtr createSliderStyle( castor::String const & name )
		{
			m_sliderStyles[name] = castor::makeUnique< SliderStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_sliderStyles[name].get();
		}

		StaticStyleRPtr createStaticStyle( castor::String const & name )
		{
			m_staticStyles[name] = castor::makeUnique< StaticStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_staticStyles[name].get();
		}

		PanelStyleRPtr createPanelStyle( castor::String const & name )
		{
			m_panelStyles[name] = castor::makeUnique< PanelStyle >( getName() + cuT( "/" ) + name
				, m_engine );
			return m_panelStyles[name].get();
		}

		ExpandablePanelStyleRPtr createExpandablePanelStyle( castor::String const & name )
		{
			m_expandablePanelStyles[name] = castor::makeUnique< ExpandablePanelStyle >( getName() + cuT( "/" ) + name
				, m_engine
				, getDefaultFont()->getName() );
			return m_expandablePanelStyles[name].get();
		}

		ButtonStyleRPtr getButtonStyle( castor::String const & name )const
		{
			auto it = m_buttonStyles.find( name );

			if ( it == m_buttonStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		ComboBoxStyleRPtr getComboBoxStyle( castor::String const & name )const
		{
			auto it = m_comboBoxStyles.find( name );

			if ( it == m_comboBoxStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		EditStyleRPtr getEditStyle( castor::String const & name )const
		{
			auto it = m_editStyles.find( name );

			if ( it == m_editStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		ListBoxStyleRPtr getListBoxStyle( castor::String const & name )const
		{
			auto it = m_listBoxStyles.find( name );

			if ( it == m_listBoxStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		SliderStyleRPtr getSliderStyle( castor::String const & name )const
		{
			auto it = m_sliderStyles.find( name );

			if ( it == m_sliderStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		StaticStyleRPtr getStaticStyle( castor::String const & name )const
		{
			auto it = m_staticStyles.find( name );

			if ( it == m_staticStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		PanelStyleRPtr getPanelStyle( castor::String const & name )const
		{
			auto it = m_panelStyles.find( name );

			if ( it == m_panelStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		ExpandablePanelStyleRPtr getExpandablePanelStyle( castor::String const & name )const
		{
			auto it = m_expandablePanelStyles.find( name );

			if ( it == m_expandablePanelStyles.end() )
			{
				CU_SrcException( "Theme", "Couldn't find style" );
			}

			return it->second.get();
		}

		template< typename StyleT >
		StyleT * getStyle( castor::String const & name )
		{
			if constexpr ( std::is_same_v< StyleT, ButtonStyle > )
			{
				return getButtonStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ComboBoxStyle > )
			{
				return getComboBoxStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, EditStyle > )
			{
				return getEditStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ListBoxStyle > )
			{
				return getListBoxStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, SliderStyle > )
			{
				return getSliderStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, StaticStyle > )
			{
				return getStaticStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, PanelStyle > )
			{
				return getPanelStyle( name );
			}
			else if constexpr ( std::is_same_v< StyleT, ExpandablePanelStyle > )
			{
				return getExpandablePanelStyle( name );
			}
			else
			{
				log::error << "Unsupported style type" << std::endl;
				return nullptr;
			}
		}

	private:
		Engine & m_engine;
		castor::FontResPtr m_defaultFont;
		std::map< castor::String, ButtonStyleUPtr > m_buttonStyles;
		std::map< castor::String, ComboBoxStyleUPtr > m_comboBoxStyles;
		std::map< castor::String, EditStyleUPtr > m_editStyles;
		std::map< castor::String, ListBoxStyleUPtr > m_listBoxStyles;
		std::map< castor::String, SliderStyleUPtr > m_sliderStyles;
		std::map< castor::String, StaticStyleUPtr > m_staticStyles;
		std::map< castor::String, PanelStyleUPtr > m_panelStyles;
		std::map< castor::String, ExpandablePanelStyleUPtr > m_expandablePanelStyles;
	};
}

#endif
