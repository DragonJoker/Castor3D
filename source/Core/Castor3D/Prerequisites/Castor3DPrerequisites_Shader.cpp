#include "Castor3D/Castor3DPrerequisites.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr PassBufferIndex = 0u;
		static uint32_t constexpr TexturesBufferIndex = 1u;
		static uint32_t constexpr LightBufferIndex = 2u;
		static uint32_t constexpr MinBufferIndex = 3u;
		static uint32_t constexpr MinTextureIndex = 3u;
	}

	constexpr uint32_t getPassBufferIndex()noexcept
	{
		return PassBufferIndex;
	}

	constexpr uint32_t getTexturesBufferIndex()noexcept
	{
		return TexturesBufferIndex;
	}

	constexpr uint32_t getLightBufferIndex()noexcept
	{
		return LightBufferIndex;
	}

	constexpr uint32_t getMinBufferIndex()noexcept
	{
		return MinBufferIndex;
	}

	constexpr uint32_t getMinTextureIndex()noexcept
	{
		return MinTextureIndex;
	}

	//*************************************************************************

	ShaderModule::ShaderModule( ShaderModule && rhs )
		: stage{ rhs.stage }
		, name{ std::move( rhs.name ) }
		, source{ std::move( rhs.source ) }
		, shader{ std::move( rhs.shader ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )
	{
		stage = rhs.stage;
		name = std::move( rhs.name );
		source = std::move( rhs.source );
		shader = std::move( rhs.shader );

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
		, source{ source }
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

	namespace shader
	{
		//*********************************************************************

		std::unique_ptr< Materials > createMaterials( sdw::ShaderWriter & writer
			, PassFlags const & passFlags )
		{
			std::unique_ptr< Materials > result;

			if ( checkFlag( passFlags, PassFlag::eMetallicRoughness ) )
			{
				result = std::make_unique< PbrMRMaterials >( writer );
			}
			else if ( checkFlag( passFlags, PassFlag::eSpecularGlossiness ) )
			{
				result = std::make_unique< PbrSGMaterials >( writer );
			}
			else
			{
				result = std::make_unique< LegacyMaterials >( writer );
			}

			return result;
		}
	}

	//*************************************************************************
}
