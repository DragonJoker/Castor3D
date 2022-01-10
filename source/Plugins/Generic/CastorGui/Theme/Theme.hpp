/*
See LICENSE file in root folder
*/
#ifndef ___CI_Theme_H___
#define ___CI_Theme_H___

#include "StyleButton.hpp"
#include "StyleComboBox.hpp"
#include "StyleEdit.hpp"
#include "StyleListBox.hpp"
#include "StyleSlider.hpp"
#include "StyleStatic.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

namespace CastorGui
{
	/**
	 *\brief	Regroups styles for all control types.
	*/
	class Theme
		: public castor::Named
	{
	public:
		Theme( castor::String const & name
			, castor3d::Engine & engine )
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
		*\param[in] p_font
		*	The font
		*/
		void setDefaultFont( castor::FontResPtr font )
		{
			m_defaultFont = font;
		}

		ButtonStyleRPtr createButtonStyle()
		{
			m_buttonStyle = std::make_unique< ButtonStyle >( getName() + "/Button"
				, m_engine
				, getDefaultFont()->getName() );
			return m_buttonStyle.get();
		}

		ComboBoxStyleRPtr createComboBoxStyle()
		{
			m_comboBoxStyle = std::make_unique< ComboBoxStyle >( getName() + "/ComboBox"
				, m_engine
				, getDefaultFont()->getName() );
			return m_comboBoxStyle.get();
		}

		EditStyleRPtr createEditStyle()
		{
			m_editStyle = std::make_unique< EditStyle >( getName() + "/Edit"
				, m_engine
				, getDefaultFont()->getName() );
			return m_editStyle.get();
		}

		ListBoxStyleRPtr createListBoxStyle()
		{
			m_listBoxStyle = std::make_unique< ListBoxStyle >( getName() + "/ListBox"
				, m_engine
				, getDefaultFont()->getName() );
			return m_listBoxStyle.get();
		}

		SliderStyleRPtr createSliderStyle()
		{
			m_sliderStyle = std::make_unique< SliderStyle >( getName() + "/Slider"
				, m_engine
				, getDefaultFont()->getName() );
			return m_sliderStyle.get();
		}

		StaticStyleRPtr createStaticStyle()
		{
			m_staticStyle = std::make_unique< StaticStyle >( getName() + "/Static"
				, m_engine
				, getDefaultFont()->getName() );
			return m_staticStyle.get();
		}

		ButtonStyleRPtr getButtonStyle()const
		{
			return m_buttonStyle.get();
		}

		ComboBoxStyleRPtr getComboBoxStyle()const
		{
			return m_comboBoxStyle.get();
		}

		EditStyleRPtr getEditStyle()const
		{
			return m_editStyle.get();
		}

		ListBoxStyleRPtr getListBoxStyle()const
		{
			return m_listBoxStyle.get();
		}

		SliderStyleRPtr getSliderStyle()const
		{
			return m_sliderStyle.get();
		}

		StaticStyleRPtr getStaticStyle()const
		{
			return m_staticStyle.get();
		}

	private:
		castor3d::Engine & m_engine;
		castor::FontResPtr m_defaultFont;
		ButtonStyleUPtr m_buttonStyle;
		ComboBoxStyleUPtr m_comboBoxStyle;
		EditStyleUPtr m_editStyle;
		ListBoxStyleUPtr m_listBoxStyle;
		SliderStyleUPtr m_sliderStyle;
		StaticStyleUPtr m_staticStyle;
	};
}

#endif
