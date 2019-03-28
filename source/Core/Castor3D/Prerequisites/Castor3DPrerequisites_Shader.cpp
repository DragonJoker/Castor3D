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

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
		, std::string const & name )
		: stage{ stage }
		, name{ name }
	{
	}

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
		, std::string const & name
		, std::string source )
		: stage{ stage }
		, name{ name }
		, source{ source }
	{
	}

	ShaderModule::ShaderModule( ashes::ShaderStageFlag stage
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
