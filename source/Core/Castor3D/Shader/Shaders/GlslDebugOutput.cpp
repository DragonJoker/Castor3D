#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"

#include "Castor3D/Miscellaneous/DebugConfig.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	DebugOutput::DebugOutput( DebugConfig & config
		, String category
		, sdw::UInt const & index
		, sdw::Vec4 const & output
		, bool enable )
		: m_config{ config }
		, m_categories{ c3d::move( category ) }
		, m_index{ index }
		, m_output{ output }
		, m_enable{ enable }
		, m_value{ sdw::findWriterMandat( m_index, m_output ).declGlobal( "c3d_debugValue", sdw::vec3( 0.0_f ), m_enable ) }
		, m_valueSet{ sdw::findWriterMandat( m_index, m_output ).declGlobal( "c3d_debugIndices", 0_u, m_enable ) }
	{
	}

	DebugOutput::~DebugOutput()noexcept
	{
		if ( m_enable )
		{
			auto & writer = sdw::findWriterMandat( m_index, m_output );

			sdwIF( writer, m_valueSet )
			{
				auto value = writer.declLocale( "debugValue", m_value );
				m_output.xyz() = value;
			}
			sdwFI
		}
	}

	void DebugOutput::registerOutput( String const & category
		, String const & name
		, sdw::Vec4 const & value )
	{
		if ( m_enable )
			registerOutput( category, name, value.xyz() );
	}

	void DebugOutput::registerOutput( String const & category
		, String const & name
		, sdw::Vec3 const & value )
	{
		if ( m_enable )
		{
			auto index = m_config.registerValue( category, name );
			auto & writer = sdw::findWriterMandat( m_index, m_output );

			sdwIF( writer, m_index == index )
			{
				m_value = value;
				m_valueSet = 1_u;
			}
			sdwFI
		}
	}

	void DebugOutput::registerOutput( String const & category
		, String const & name
		, sdw::Vec2 const & value )
	{
		if ( m_enable )
			registerOutput( category, name, vec3( value, 0.0_f ) );
	}

	void DebugOutput::registerOutput( String const & category
		, String const & name
		, sdw::Float const & value )
	{
		if ( m_enable )
			registerOutput( category, name, vec3( value ) );
	}

	DebugOutputCategory DebugOutput::pushBlock( String category )
	{
		m_categories.push_back( c3d::move( category ) );
		return DebugOutputCategory{ *this };
	}

	String DebugOutput::concatenateCategories()const
	{
		String result;
		String sep;

		for ( auto const & category : m_categories )
		{
			result += sep + category;
			sep = "/";
		}

		return result;
	}
}
