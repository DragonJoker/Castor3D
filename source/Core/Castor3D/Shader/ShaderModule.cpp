#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

CU_ImplementSmartPtr( castor3d::shader, BufferBase )

namespace castor3d
{
	//*************************************************************************

	ShaderModule::ShaderModule( ShaderModule && rhs )noexcept
		: stage{ rhs.stage }
		, name{ std::move( rhs.name ) }
		, source{ std::move( rhs.source ) }
		, shader{ std::move( rhs.shader ) }
		, compiled{ std::move( rhs.compiled ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )noexcept
	{
		stage = rhs.stage;
		name = std::move( rhs.name );
		source = std::move( rhs.source );
		shader = std::move( rhs.shader );
		compiled = std::move( rhs.compiled );

		return *this;
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, std::string const & name )
		: stage{ stage }
		, name{ name }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, std::string const & name
		, std::string source )
		: stage{ stage }
		, name{ name }
		, source{ std::move( source ) }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, std::string const & name
		, ShaderPtr shader )
		: stage{ stage }
		, name{ name }
		, shader{ std::move( shader ) }
	{
	}

	//*************************************************************************

	ProgramModule::ProgramModule( ProgramModule && rhs )noexcept
		: name{ std::move( rhs.name ) }
		, shader{ std::move( rhs.shader ) }
		, compiled{ std::move( rhs.compiled ) }
	{
	}

	ProgramModule & ProgramModule::operator=( ProgramModule && rhs )noexcept
	{
		name = std::move( rhs.name );
		shader = std::move( rhs.shader );
		compiled = std::move( rhs.compiled );

		return *this;
	}

	ProgramModule::ProgramModule( std::string const & pname )
		: name{ pname }
	{
	}

	ProgramModule::ProgramModule( std::string const & pname
		, ShaderPtr pshader )
		: name{ pname }
		, shader{ std::move( pshader ) }
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

		castor::String concatModelNames( castor::String lhs
			, castor::String rhs )
		{
			castor::string::replace( lhs, cuT( "c3d." ), castor::cuEmptyString );
			castor::string::replace( rhs, cuT( "c3d." ), castor::cuEmptyString );
			return cuT( "c3d." ) + lhs + cuT( "." ) + rhs;
		}
	}

	//*************************************************************************
}
