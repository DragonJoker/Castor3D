/*
See LICENSE file in root folder
*/
#ifndef ___CI_ControlStyle_H___
#define ___CI_ControlStyle_H___

#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/UserInput/EventHandler.hpp>

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Pixel.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace CastorGui
{
	/**
	*\brief
	*	Description of the style of a control
	*/
	class ControlStyle
		: public castor::Named
	{
	public:
		virtual ~ControlStyle() = default;

		ControlStyle( ControlType type
			, castor::String const & name
			, castor3d::Engine & engine
			, MouseCursor cursor = MouseCursor::eHand )
			: castor::Named{ name }
			, m_engine{ engine }
			, m_type{ type }
			, m_cursor{ cursor }
			, m_backgroundMaterial{ getEngine().getMaterialCache().find( cuT( "Black" ) ).lock().get() }
			, m_foregroundMaterial{ getEngine().getMaterialCache().find( cuT( "White" ) ).lock().get() }
		{
		}

		void setBackgroundMaterial( castor3d::MaterialRPtr value )
		{
			m_backgroundMaterial = value;
			doUpdateBackgroundMaterial();
		}

		void setForegroundMaterial( castor3d::MaterialRPtr value )
		{
			m_foregroundMaterial = value;
			doUpdateForegroundMaterial();
		}

		void setCursor( MouseCursor value )
		{
			m_cursor = value;
		}

		castor3d::MaterialRPtr getBackgroundMaterial()const
		{
			return m_backgroundMaterial;
		}

		castor3d::MaterialRPtr getForegroundMaterial()const
		{
			return m_foregroundMaterial;
		}

		MouseCursor getCursor()const
		{
			return m_cursor;
		}

		castor3d::Engine & getEngine()const
		{
			return m_engine;
		}

	protected:
		castor3d::MaterialRPtr doCreateMaterial( castor3d::MaterialRPtr material
			, float offset
			, castor::String const & suffix )
		{
			castor::RgbColour colour = getMaterialColour( *material->getPass( 0u ) );
			colour.red() = float( colour.red() ) + offset;
			colour.green() = float( colour.green() ) + offset;
			colour.blue() = float( colour.blue() ) + offset;
			return createMaterial( getEngine(), material->getName() + suffix, colour );
		}

	private:
		virtual void doUpdateBackgroundMaterial() = 0;
		virtual void doUpdateForegroundMaterial() = 0;

	private:
		castor3d::Engine & m_engine;
		const ControlType m_type;
		MouseCursor m_cursor;
		castor3d::MaterialRPtr m_backgroundMaterial{};
		castor3d::MaterialRPtr m_foregroundMaterial{};
	};
}

#endif
