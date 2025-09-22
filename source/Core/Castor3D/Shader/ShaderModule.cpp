#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>
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

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Int const in )
		{
			return vec3( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::IVec2 const in )
		{
			return vec3( vec2( in ), 0.0_f );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::IVec3 const in )
		{
			return vec3( in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::IVec4 const in )
		{
			return vec3( in.xyz() );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UInt const in )
		{
			return vec3( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::UVec2 const in )
		{
			return vec3( vec2( in ), 0.0_f );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::UVec3 const in )
		{
			return vec3( in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::UVec4 const in )
		{
			return vec3( in.xyz() );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Float const in )
		{
			return vec3( in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec2 const in )
		{
			return vec3( in, 0.0_f );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec3 const in )
		{
			return in;
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::Vec4 const in )
		{
			return in.xyz();
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Double const in )
		{
			return vec3( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::DVec2 const in )
		{
			return vec3( vec2( in ), 0.0_f );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::DVec3 const in )
		{
			return vec3( in );
		}

		sdw::Vec3 makeVec3( sdw::ShaderWriter const &, sdw::DVec4 const in )
		{
			return vec3( in.xyz() );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Int const in )
		{
			return vec4( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::IVec2 const in )
		{
			return vec4( vec2( in ), 0.0_f, 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::IVec3 const in )
		{
			return vec4( vec3( in ), 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::IVec4 const in )
		{
			return vec4( vec3( in.xyz() ), 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UInt const in )
		{
			return vec4( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::UVec2 const in )
		{
			return vec4( vec2( in ), 0.0_f, 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::UVec3 const in )
		{
			return vec4( vec3( in ), 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::UVec4 const in )
		{
			return vec4( vec3( in.xyz() ), 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::Float const in )
		{
			return vec4( in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::Vec2 const in )
		{
			return vec4( in, 0.0_f, 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::Vec3 const in )
		{
			return vec4( in, 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::Vec4 const in )
		{
			return vec4( in );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Double const in )
		{
			return vec4( writer.cast< sdw::Float >( in ) );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::DVec2 const in )
		{
			return vec4( vec2( in ), 0.0_f, 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::DVec3 const in )
		{
			return vec4( vec3( in ), 1.0_f );
		}

		sdw::Vec4 makeVec4( sdw::ShaderWriter const &, sdw::DVec4 const in )
		{
			return vec4( vec3( in.xyz() ), 1.0_f );
		}
	}

	//*************************************************************************
}
