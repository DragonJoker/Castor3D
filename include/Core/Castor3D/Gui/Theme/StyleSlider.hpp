/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SliderStyle_H___
#define ___C3D_SliderStyle_H___

#include "StyleStatic.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class SliderStyle
		: public ControlStyle
	{
	public:
		static ControlType constexpr Type = ControlType::eSlider;

		SliderStyle( castor::String const & name
			, Scene * scene
			, Engine & engine
			, castor::String const & fontName )
			: ControlStyle{ Type
				, name
				, scene
				, engine }
			, m_lineStyle{ name + cuT( "/Tick" ), scene, engine, fontName }
			, m_tickStyle{ name + cuT( "/Line" ), scene, engine, fontName }
		{
		}

		SliderStyle( castor::String const & name
			, Engine & engine
			, castor::String const & fontName )
			: SliderStyle{ name, nullptr, engine, fontName }
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
