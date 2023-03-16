/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PanelStyle_H___
#define ___C3D_PanelStyle_H___

#include "StyleControl.hpp"

namespace castor3d
{
	class StylesHolder
	{
	public:
		C3D_API virtual ~StylesHolder()noexcept = default;

		explicit StylesHolder( castor::String name
			, Engine & engine )
			: m_engine{ engine }
			, m_holderName{ std::move( name ) }
			, m_defaultFont{ engine.getFontCache().isEmpty()
			? nullptr
			: engine.getFontCache().begin()->second }
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

		ButtonStyleRPtr createButtonStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_buttonStyles );
		}

		ComboBoxStyleRPtr createComboBoxStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_comboBoxStyles );
		}

		EditStyleRPtr createEditStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_editStyles );
		}

		ListBoxStyleRPtr createListBoxStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_listBoxStyles );
		}

		SliderStyleRPtr createSliderStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_sliderStyles );
		}

		StaticStyleRPtr createStaticStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_staticStyles );
		}

		PanelStyleRPtr createPanelStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_panelStyles );
		}

		ExpandablePanelStyleRPtr createExpandablePanelStyle( castor::String name )
		{
			return createControlStyle( std::move( name ), m_expandablePanelStyles );
		}

		virtual ButtonStyleRPtr getButtonStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_buttonStyles );
		}

		virtual ComboBoxStyleRPtr getComboBoxStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_comboBoxStyles );
		}

		virtual EditStyleRPtr getEditStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_editStyles );
		}

		virtual ListBoxStyleRPtr getListBoxStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_listBoxStyles );
		}

		virtual SliderStyleRPtr getSliderStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_sliderStyles );
		}

		virtual StaticStyleRPtr getStaticStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_staticStyles );
		}

		virtual PanelStyleRPtr getPanelStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_panelStyles );
		}

		virtual ExpandablePanelStyleRPtr getExpandablePanelStyle( castor::String const & name )const
		{
			return getControlStyle( name, m_expandablePanelStyles );
		}

		template< typename StyleT >
		StyleT * getStyle( castor::String const & name )const
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
		template< typename StyleT >
		StyleT * createControlStyle( castor::String name
			, std::map< castor::String, castor::UniquePtr< StyleT > > & controls );

		template< typename StyleT >
		StyleT * getControlStyle( castor::String name
			, std::map< castor::String, castor::UniquePtr< StyleT > > const & controls )const;

	private:
		Engine & m_engine;
		castor::String m_holderName;
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

	class PanelStyle
		: public ControlStyle
		, public StylesHolder
	{
	public:
		static ControlType constexpr Type = ControlType::ePanel;

		PanelStyle( castor::String const & name
			, Engine & engine )
			: ControlStyle{ Type, name, engine }
			, StylesHolder{ name, engine }
		{
		}


	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
		}
	};
}

#endif
