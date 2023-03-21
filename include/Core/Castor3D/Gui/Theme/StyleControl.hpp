/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ControlStyle_H___
#define ___C3D_ControlStyle_H___

#include "Castor3D/Gui/GuiModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/UserInput/EventHandler.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Pixel.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	class ControlStyle
		: public castor::Named
	{
	public:
		C3D_API virtual ~ControlStyle()noexcept = default;

		ControlStyle( ControlType type
			, castor::String const & name
			, Scene * scene
			, Engine & engine
			, MouseCursor cursor = MouseCursor::eArrow )
			: castor::Named{ name }
			, m_scene{ scene }
			, m_engine{ engine }
			, m_type{ type }
			, m_cursor{ cursor }
			, m_backgroundMaterial{ getEngine().findMaterial( cuT( "Black" ) ).lock().get() }
			, m_foregroundMaterial{ getEngine().findMaterial( cuT( "White" ) ).lock().get() }
		{
		}

		void setBackgroundMaterial( MaterialRPtr value )
		{
			m_backgroundMaterial = value;
			doUpdateBackgroundMaterial();
		}

		void setForegroundMaterial( MaterialRPtr value )
		{
			m_foregroundMaterial = value;
			doUpdateForegroundMaterial();
		}

		void setCursor( MouseCursor value )noexcept
		{
			m_cursor = value;
		}

		void setBackgroundInvisible( bool value )noexcept
		{
			m_backgroundInvisible = value;
		}

		void setForegroundInvisible( bool value )noexcept
		{
			m_foregroundInvisible = value;
		}

		bool isBackgroundInvisible()const noexcept
		{
			return m_backgroundInvisible;
		}

		bool isForegroundInvisible()const noexcept
		{
			return m_foregroundInvisible;
		}

		MaterialRPtr getBackgroundMaterial()const noexcept
		{
			return m_backgroundMaterial;
		}

		MaterialRPtr getForegroundMaterial()const noexcept
		{
			return m_foregroundMaterial;
		}

		ControlType getType()const noexcept
		{
			return m_type;
		}

		MouseCursor getCursor()const noexcept
		{
			return m_cursor;
		}

		Engine & getEngine()const noexcept
		{
			return m_engine;
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

	protected:
		MaterialRPtr doCreateMaterial( MaterialRPtr material
			, float offset
			, castor::String const & suffix )
		{
			if ( !material )
			{
				return nullptr;
			}

			auto colour = getMaterialColour( *material->getPass( 0u ) );
			colour.red() = float( colour.red() ) + offset;
			colour.green() = float( colour.green() ) + offset;
			colour.blue() = float( colour.blue() ) + offset;
			auto result = createMaterial( getEngine(), material->getName() + suffix, colour );
			result->setSerialisable( false );
			return result;
		}

	private:
		virtual void doUpdateBackgroundMaterial() = 0;
		virtual void doUpdateForegroundMaterial() = 0;

	private:
		Scene * m_scene{};
		Engine & m_engine;
		ControlType m_type{};
		MouseCursor m_cursor{};
		MaterialRPtr m_backgroundMaterial{};
		MaterialRPtr m_foregroundMaterial{};
		bool m_backgroundInvisible{};
		bool m_foregroundInvisible{};
	};
}

#endif
