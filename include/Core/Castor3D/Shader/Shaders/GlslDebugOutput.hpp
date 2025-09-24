/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDebugOutput_H___
#define ___C3D_GlslDebugOutput_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <unordered_set>
#include <vector>

namespace c3d::shader
{
	class DebugOutput
	{
	public:
		C3D_API DebugOutput( DebugConfig & config
			, String category
			, sdw::UInt const & index
			, sdw::Vec4 const & output
			, bool enable );
		C3D_API ~DebugOutput()noexcept;

		C3D_API void registerOutput( String const & category
			, String const & name
			, sdw::Vec4 const & value );
		C3D_API void registerOutput( String const & category
			, String const & name
			, sdw::Vec3 const & value );
		C3D_API void registerOutput( String const & category
			, String const & name
			, sdw::Vec2 const & value );
		C3D_API void registerOutput( String const & category
			, String const & name
			, sdw::Float const & value );
		C3D_API DebugOutputCategory pushBlock( String category );

		template< typename ValueT >
		void registerOutput( String const & name
			, ValueT const & value )
		{
			registerOutput( concatenateCategories(), name, value );
		}

		bool isEnabled()const
		{
			return m_enable;
		}

	private:
		friend class DebugOutputCategory;

		C3D_API String concatenateCategories()const;

		void popBlock()
		{
			m_categories.pop_back();
		}

	private:
		DebugConfig & m_config;
		StringArray m_categories;
		sdw::UInt m_index;
		sdw::Vec4 m_output;
		bool m_enable;
		sdw::Vec3 m_value;
		sdw::UInt m_valueSet;
		sdw::Function< sdw::Void
			, sdw::InUInt
			, sdw::InVec3 > m_registerOutput;
	};

	class DebugOutputCategory
	{
	public:
		DebugOutputCategory( DebugOutputCategory const & rhs ) = delete;
		DebugOutputCategory& operator=( DebugOutputCategory const & rhs ) = delete;

		explicit DebugOutputCategory( DebugOutput & debugOutput )noexcept
			: m_debugOutput{ &debugOutput }
		{
		}

		DebugOutputCategory( DebugOutputCategory && rhs )noexcept
			: m_debugOutput{ rhs.m_debugOutput }
		{
			rhs.m_debugOutput = {};
		}

		~DebugOutputCategory()noexcept
		{
			if ( m_debugOutput )
			{
				m_debugOutput->popBlock();
			}
		}

		DebugOutputCategory & operator=( DebugOutputCategory && rhs )noexcept
		{
			m_debugOutput = rhs.m_debugOutput;

			rhs.m_debugOutput = {};

			return *this;
		}

		DebugOutput & operator*()const noexcept
		{
			return *m_debugOutput;
		}

		bool isEnabled()const
		{
			return m_debugOutput->isEnabled();
		}

		DebugOutputCategory pushBlock( String category )const
		{
			return m_debugOutput->pushBlock( c3d::move( category ) );
		}

		template< typename ValueT >
		void registerOutput( String name
			, ValueT const & value )const
		{
			m_debugOutput->registerOutput( c3d::move( name ), value );
		}

	private:
		DebugOutput * m_debugOutput;
	};
}

#endif
