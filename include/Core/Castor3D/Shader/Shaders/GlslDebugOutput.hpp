/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDebugOutput_H___
#define ___C3D_GlslDebugOutput_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#include <vector>

namespace castor3d::shader
{
	class DebugOutput
	{
	public:
		C3D_API DebugOutput( DebugConfig & config
			, castor::String category
			, sdw::UInt const index
			, sdw::Vec4 const output
			, bool enable );
		C3D_API ~DebugOutput()noexcept;

		C3D_API void registerOutput( castor::String category
			, castor::String name
			, sdw::Vec4 const value );
		C3D_API void registerOutput( castor::String category
			, castor::String name
			, sdw::Vec3 const value );
		C3D_API void registerOutput( castor::String category
			, castor::String name
			, sdw::Vec2 const value );
		C3D_API void registerOutput( castor::String category
			, castor::String name
			, sdw::Float const value );

		void registerOutput( castor::String name
			, sdw::Vec4 const value )
		{
			registerOutput( m_category, name, value );
		}

		void registerOutput( castor::String name
			, sdw::Vec3 const value )
		{
			registerOutput( m_category, name, value );
		}

		void registerOutput( castor::String name
			, sdw::Vec2 const value )
		{
			registerOutput( m_category, name, value );
		}

		void registerOutput( castor::String name
			, sdw::Float const value )
		{
			registerOutput( m_category, name, value );
		}

	private:
		DebugConfig & m_config;
		castor::String m_category;
		sdw::UInt m_index;
		sdw::Vec4 m_output;
		bool m_enable;
		std::vector< std::pair< uint32_t, sdw::Vec4 > > m_values;
	};
}

#endif
