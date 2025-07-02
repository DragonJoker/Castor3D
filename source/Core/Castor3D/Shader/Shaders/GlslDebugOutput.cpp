#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"

#include <CastorUtils/Data/Path.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	DebugOutput::DebugOutput( DebugConfig & config
		, castor::String category
		, sdw::UInt const index
		, sdw::Vec4 const output
		, bool enable )
		: m_config{ config }
		, m_categories{ castor::move( category ) }
		, m_index{ index }
		, m_output{ output }
		, m_enable{ enable }
		, m_values{ sdw::findWriterMandat( m_index, m_output ).declGlobalArray< sdw::Vec3 >( "c3d_debugValue", 512u, m_enable ) }
		, m_indices{ sdw::findWriterMandat( m_index, m_output ).declGlobalArray< sdw::UInt >( "c3d_debugIndices", 512u, m_enable ) }
	{
		if ( m_enable )
		{
			auto & writer = sdw::findWriterMandat( m_index, m_output );
			sdwFOR( writer, sdw::UInt, i, 0_u, i < 512_u, ++i )
			{
				m_indices[i] = 0_u;
			}
			sdwROF;
			m_registerOutput = writer.implementFunction< sdw::Void >( "c3d_registerOutput"
				, [this]( sdw::UInt const & idx
					, sdw::Vec3 const & val )
				{
					m_indices[idx] = 1_u;
					m_values[idx] = val;
				}
				, sdw::InUInt{ writer, "index" }
				, sdw::InVec3{ writer, "value" } );
		}
	}

	DebugOutput::~DebugOutput()noexcept
	{
		if ( m_enable )
		{
			auto & writer = sdw::findWriterMandat( m_index, m_output );

			sdwIF( writer, m_index != 0_u
				&& m_indices[m_index] != 0_u )
			{
				auto value = writer.declLocale( "debugValue", m_values[m_index] );
				m_output.xyz() = value;
			}
			sdwFI;
		}
	}

	void DebugOutput::registerOutput( castor::String category
		, castor::String name
		, sdw::Vec4 const value )
	{
		if ( m_enable )
			registerOutput( category, name, value.xyz() );
	}

	void DebugOutput::registerOutput( castor::String category
		, castor::String name
		, sdw::Vec3 const value )
	{
		if ( m_enable )
		{
			auto index = m_config.registerValue( category, name );
			m_registerOutput( sdw::UInt{ index }, value );
		}
	}

	void DebugOutput::registerOutput( castor::String category
		, castor::String name
		, sdw::Vec2 const value )
	{
		if ( m_enable )
			registerOutput( category, name, vec3( value, 0.0_f ) );
	}

	void DebugOutput::registerOutput( castor::String category
		, castor::String name
		, sdw::Float const value )
	{
		if ( m_enable )
			registerOutput( category, name, vec3( value ) );
	}

	DebugOutputCategory DebugOutput::pushBlock( castor::String category )
	{
		m_categories.push_back( castor::move( category ) );
		return DebugOutputCategory{ *this };
	}

	castor::String DebugOutput::concatenateCategories()const
	{
		castor::String result;
		castor::String sep;

		for ( auto const & category : m_categories )
		{
			result += sep + category;
			sep = "/";
		}

		return result;
	}
}
