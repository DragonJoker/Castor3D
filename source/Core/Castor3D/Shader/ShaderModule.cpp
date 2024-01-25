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
		, name{ castor::move( rhs.name ) }
		, source{ castor::move( rhs.source ) }
		, shader{ castor::move( rhs.shader ) }
		, compiled{ castor::move( rhs.compiled ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )noexcept
	{
		stage = rhs.stage;
		name = castor::move( rhs.name );
		source = castor::move( rhs.source );
		shader = castor::move( rhs.shader );
		compiled = castor::move( rhs.compiled );

		return *this;
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, castor::String const & name )
		: stage{ stage }
		, name{ name }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, castor::String const & name
		, castor::MbString source )
		: stage{ stage }
		, name{ name }
		, source{ castor::move( source ) }
	{
	}

	ShaderModule::ShaderModule( VkShaderStageFlagBits stage
		, castor::String const & name
		, ShaderPtr shader )
		: stage{ stage }
		, name{ name }
		, shader{ castor::move( shader ) }
	{
	}

	//*************************************************************************

	ProgramModule::ProgramModule( ProgramModule && rhs )noexcept
		: name{ castor::move( rhs.name ) }
		, shader{ castor::move( rhs.shader ) }
		, compiled{ castor::move( rhs.compiled ) }
	{
	}

	ProgramModule & ProgramModule::operator=( ProgramModule && rhs )noexcept
	{
		name = castor::move( rhs.name );
		shader = castor::move( rhs.shader );
		compiled = castor::move( rhs.compiled );

		return *this;
	}

	ProgramModule::ProgramModule( castor::String const & pname )
		: name{ pname }
	{
	}

	ProgramModule::ProgramModule( castor::String const & pname
		, ShaderPtr pshader )
		: name{ pname }
		, shader{ castor::move( pshader ) }
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
			castor::string::replace( lhs, cuT( "c3d." ), cuT( "" ) );
			castor::string::replace( rhs, cuT( "c3d." ), cuT( "" ) );
			return cuT( "c3d." ) + lhs + cuT( "." ) + rhs;
		}
	}

	//*************************************************************************
}
