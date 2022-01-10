/*
See LICENSE file in root folder
*/
#ifndef ___CI_SliderStyle_H___
#define ___CI_SliderStyle_H___

#include "StyleStatic.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace CastorGui
{
	/**
	 *\brief	Slider style.
	*/
	class SliderStyle
		: public ControlStyle
	{
	public:
		SliderStyle( castor::String const & name
			, castor3d::Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ ControlType::eSlider
				, name
				, engine }
			, m_lineStyle{ name + "/Tick", engine, fontName }
			, m_tickStyle{ name + "/Line", engine, fontName }
		{
		}

		StaticStyle const & getLineStyle()const
		{
			return m_lineStyle;
		}

		StaticStyle const & getTickStyle()const
		{
			return m_tickStyle;
		}

		StaticStyle & getLineStyle()
		{
			return m_lineStyle;
		}

		StaticStyle & getTickStyle()
		{
			return m_tickStyle;
		}

	private:
		void doUpdateBackgroundMaterial()override
		{
		}

		void doUpdateForegroundMaterial()override
		{
			m_lineStyle.setForegroundMaterial( getForegroundMaterial() );
			m_tickStyle.setForegroundMaterial( getForegroundMaterial() );
		}

	private:
		StaticStyle m_lineStyle;
		StaticStyle m_tickStyle;
	};
}

#endif
