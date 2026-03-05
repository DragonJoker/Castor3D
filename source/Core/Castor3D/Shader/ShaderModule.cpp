#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Shader/Shaders/GlslBuffer.hpp"

#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

CU_ImplementSmartPtr( c3d::shader, BufferBase )

namespace c3d
{
	//*************************************************************************

	ShaderModule::ShaderModule( ShaderModule && rhs )noexcept
		: stage{ rhs.stage }
		, name{ c3d::move( rhs.name ) }
		, source{ c3d::move( rhs.source ) }
		, shader{ c3d::move( rhs.shader ) }
		, compiled{ c3d::move( rhs.compiled ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )noexcept
	{
		stage = rhs.stage;
		name = c3d::move( rhs.name );
		source = c3d::move( rhs.source );
		shader = c3d::move( rhs.shader );
		compiled = c3d::move( rhs.compiled );

		return *this;
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, String const & name )
		: stage{ stage }
		, name{ name }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, String const & name
		, MbString source )
		: stage{ stage }
		, name{ name }
		, source{ c3d::move( source ) }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, String const & name
		, ShaderPtr shader )
		: stage{ stage }
		, name{ name }
		, shader{ c3d::move( shader ) }
	{
	}

	//*************************************************************************

	ProgramModule::ProgramModule( ProgramModule && rhs )noexcept
		: name{ c3d::move( rhs.name ) }
		, shader{ c3d::move( rhs.shader ) }
		, compiled{ c3d::move( rhs.compiled ) }
	{
	}

	ProgramModule & ProgramModule::operator=( ProgramModule && rhs )noexcept
	{
		name = c3d::move( rhs.name );
		shader = c3d::move( rhs.shader );
		compiled = c3d::move( rhs.compiled );

		return *this;
	}

	ProgramModule::ProgramModule( String const & pname )
		: name{ pname }
	{
	}

	ProgramModule::ProgramModule( String const & pname
		, ShaderPtr pshader )
		: name{ pname }
		, shader{ c3d::move( pshader ) }
	{
	}

	//*************************************************************************

	namespace shader
	{
		uint32_t getSpotShadowMapCount()
		{
			return MaxSpotShadowMapCount;
		}

		uint32_t getPointShadowMapCount()
		{
			return MaxPointShadowMapCount;
		}

		void groupMemoryBarrierWithGroupSync( sdw::ShaderWriter & writer )
		{
			writer.controlBarrier( sdw::type::Scope::eWorkgroup
				, sdw::type::Scope::eWorkgroup
				, ( sdw::type::MemorySemanticsMask::eAcquireRelease
					| sdw::type::MemorySemanticsMask::eWorkgroupMemory ) );
		}

		String concatModelNames( String lhs
			, String rhs )
		{
			string::replace( lhs, cuT( "c3d." ), cuT( "" ) );
			string::replace( rhs, cuT( "c3d." ), cuT( "" ) );
			return cuT( "c3d." ) + lhs + cuT( "." ) + rhs;
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Int const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec2 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec3 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec4 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UInt const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec2 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec3 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec4 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Float const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec2 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec3 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec4 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Double const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec2 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec3 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec4 const & in )
		{
			return makeVec3T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Int const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec2 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec3 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec4 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UInt const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec2 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec3 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec4 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Float const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec2 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec3 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec4 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Double const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec2 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec3 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec4 const & in )
		{
			return makeVec4T< sdw::Float >( writer, in );
		}
	}

	//*************************************************************************
}
