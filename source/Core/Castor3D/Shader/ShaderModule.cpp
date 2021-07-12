#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"

#include <ShaderWriter/CompositeTypes/Struct.hpp>
#include <ShaderWriter/CompositeTypes/ArraySsbo.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************

	bool isShadowMapProgram( ProgramFlags const & flags )
	{
		return checkFlag( flags, ProgramFlag::eShadowMapDirectional )
			|| checkFlag( flags, ProgramFlag::eShadowMapSpot )
			|| checkFlag( flags, ProgramFlag::eShadowMapPoint );
	}

	//*************************************************************************

	ShaderModule::ShaderModule( ShaderModule && rhs )noexcept
		: stage{ rhs.stage }
		, name{ std::move( rhs.name ) }
		, source{ std::move( rhs.source ) }
		, shader{ std::move( rhs.shader ) }
	{
	}

	ShaderModule & ShaderModule::operator=( ShaderModule && rhs )noexcept
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

	//*************************************************************************

	namespace shader
	{
		namespace
		{
			static constexpr uint32_t SpotShadowMapCount = 10u;
			static constexpr uint32_t PointShadowMapCount = 6u;
			static constexpr uint32_t BaseLightComponentsCount = 5u;
#if C3D_UseTiledDirectionalShadowMap
			// DirectionalLight => BaseLightComponentsCount + 30 => BaseLightComponentsCount + 1(directionCount) + 1(tiles) + 2(splitDepths) + 2(splitScales) + (6 * 4)(transforms)
			// PointLight => BaseLightComponentsCount + 2
			// SpotLight => BaseLightComponentsCount + 8
			static constexpr uint32_t MaxLightComponentsCount = 35u;
#else
			// DirectionalLight => BaseLightComponentsCount + 19 => BaseLightComponentsCount + 1(directionCount) + 1(splitDepths) + 1(splitScales) + (4 * 4)(transforms)
			// PointLight => BaseLightComponentsCount + 2
			// SpotLight => BaseLightComponentsCount + 8
			static constexpr uint32_t MaxLightComponentsCount = 24u;
#endif
		}

		uint32_t getSpotShadowMapCount()
		{
			return SpotShadowMapCount;
		}

		uint32_t getPointShadowMapCount()
		{
			return PointShadowMapCount;
		}

		uint32_t getBaseLightComponentsCount()
		{
			return BaseLightComponentsCount;
		}

		uint32_t getMaxLightComponentsCount()
		{
			return MaxLightComponentsCount;
		}

		std::unique_ptr< Materials > createMaterials( sdw::ShaderWriter & writer
			, PassFlags const & passFlags )
		{
			return std::make_unique< Materials >( writer );
		}
	}

	//*************************************************************************
}
