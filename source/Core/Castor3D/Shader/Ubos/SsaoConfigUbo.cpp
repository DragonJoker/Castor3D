#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		SsaoConfigData::SsaoConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, projInfo{ getMember< sdw::Vec4 >( "projInfo" ) }
			, numSamples{ getMember< sdw::Int >( "numSamples" ) }
			, numSpiralTurns{ getMember< sdw::Int >( "numSpiralTurns" ) }
			, projScale{ getMember< sdw::Float >( "projScale" ) }
			, radius{ getMember< sdw::Float >( "radius" ) }
			, invRadius{ getMember< sdw::Float >( "invRadius" ) }
			, radius2{ getMember< sdw::Float >( "radius2" ) }
			, invRadius2{ getMember< sdw::Float >( "invRadius2" ) }
			, bias{ getMember< sdw::Float >( "bias" ) }
			, intensity{ getMember< sdw::Float >( "intensity" ) }
			, intensityDivR6{ getMember< sdw::Float >( "intensityDivR6" ) }
			, farPlaneZ{ getMember< sdw::Float >( "farPlaneZ" ) }
			, edgeSharpness{ getMember< sdw::Float >( "edgeSharpness" ) }
			, blurStepSize{ getMember< sdw::Int >( "blurStepSize" ) }
			, blurRadius{ getMember< sdw::Int >( "blurRadius" ) }
			, highQuality{ getMember< sdw::Int >( "highQuality" ) }
			, blurHighQuality{ getMember< sdw::Int >( "blurHighQuality" ) }
			, logMaxOffset{ getMember< sdw::Int >( "logMaxOffset" ) }
			, maxMipLevel{ getMember< sdw::Int >( "maxMipLevel" ) }
			, minRadius{ getMember< sdw::Float >( "minRadius" ) }
			, variation{ getMember< sdw::Int >( "variation" ) }
			, bendStepCount{ getMember< sdw::Int >( "bendStepCount" ) }
			, bendStepSize{ getMember< sdw::Float >( "bendStepSize" ) }
		{
		}

		SsaoConfigData & SsaoConfigData::operator=( SsaoConfigData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr SsaoConfigData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_SsaoConfigData" );

			if ( result->empty() )
			{
				result->declMember( "projInfo", ast::type::Kind::eVec4F );
				result->declMember( "numSamples", ast::type::Kind::eInt );
				result->declMember( "numSpiralTurns", ast::type::Kind::eInt );
				result->declMember( "projScale", ast::type::Kind::eFloat );
				result->declMember( "radius", ast::type::Kind::eFloat );
				result->declMember( "invRadius", ast::type::Kind::eFloat );
				result->declMember( "radius2", ast::type::Kind::eFloat );
				result->declMember( "invRadius2", ast::type::Kind::eFloat );
				result->declMember( "bias", ast::type::Kind::eFloat );
				result->declMember( "intensity", ast::type::Kind::eFloat );
				result->declMember( "intensityDivR6", ast::type::Kind::eFloat );
				result->declMember( "farPlaneZ", ast::type::Kind::eFloat );
				result->declMember( "edgeSharpness", ast::type::Kind::eFloat );
				result->declMember( "blurStepSize", ast::type::Kind::eInt );
				result->declMember( "blurRadius", ast::type::Kind::eInt );
				result->declMember( "highQuality", ast::type::Kind::eInt );
				result->declMember( "blurHighQuality", ast::type::Kind::eInt );
				result->declMember( "logMaxOffset", ast::type::Kind::eInt );
				result->declMember( "maxMipLevel", ast::type::Kind::eInt );
				result->declMember( "minRadius", ast::type::Kind::eFloat );
				result->declMember( "variation", ast::type::Kind::eInt );
				result->declMember( "bendStepCount", ast::type::Kind::eInt );
				result->declMember( "bendStepSize", ast::type::Kind::eFloat );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > SsaoConfigData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	String const SsaoConfigUbo::BufferSsaoConfig = cuT( "SsaoConfig" );
	String const SsaoConfigUbo::SsaoConfigData = cuT( "c3d_ssaoConfigData" );

	SsaoConfigUbo::SsaoConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}

	SsaoConfigUbo::~SsaoConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void SsaoConfigUbo::cpuUpdate( SsaoConfig const & config
		, Camera const & camera )
	{
		auto & viewport = camera.getViewport();
		int numSpiralTurns = 0;

		static constexpr uint32_t numPrecomputed = 100u;

		static int const minDiscrepancyArray[numPrecomputed]
		{
		//   0   1   2   3   4   5   6   7   8   9
			 1,  1,  1,  2,  3,  2,  5,  2,  3,  2,  // 0
			 3,  3,  5,  5,  3,  4,  7,  5,  5,  7,  // 1
			 9,  8,  5,  5,  7,  7,  7,  8,  5,  8,  // 2
			11, 12,  7, 10, 13,  8, 11,  8,  7, 14,  // 3
			11, 11, 13, 12, 13, 19, 17, 13, 11, 18,  // 4
			19, 11, 11, 14, 17, 21, 15, 16, 17, 18,  // 5
			13, 17, 11, 17, 19, 18, 25, 18, 19, 19,  // 6
			29, 21, 19, 27, 31, 29, 21, 18, 17, 29,  // 7
			31, 31, 23, 18, 25, 26, 25, 23, 19, 34,  // 8
			19, 27, 21, 25, 39, 29, 17, 21, 27, 29   // 9
		};

		if ( config.numSamples < numPrecomputed )
		{
			numSpiralTurns =  minDiscrepancyArray[config.numSamples];
		}
		else
		{
			numSpiralTurns = 5779; // Some large prime. Hope it does alright. It'll at least never degenerate into a perfect line until we have 5779 samples...
		}

		float const radius = config.radius;
		float const invRadius = 1 / radius;
		float const radius2 = radius * radius;
		float const invRadius2 = 1.0f / radius2;
		float const intersityDivR6 = config.intensity / std::pow( radius, 6.0f );

		auto & configuration = m_ubo.getData();
		float const projScale = viewport.getProjectionScale();
		float const MIN_AO_SS_RADIUS = 1.0f;
		// Second parameter of max is just solving for Z coordinate at which we hit MIN_AO_SS_RADIUS
		float farZ = std::max( viewport.getFar(), -projScale * radius / MIN_AO_SS_RADIUS );
		// Hack because setting farZ lower results in banding artefacts on some scenes, should tune later.
		farZ = std::min( farZ, -1000.0f );
		auto const & proj = camera.getProjection( true );
		configuration.projInfo = castor::Point4f
		{
			-2.0f / ( float( viewport.getWidth() ) * proj[0][0] ),
			-2.0f / ( float( viewport.getHeight() ) * proj[1][1] ),
			( 1.0f - proj[0][2] ) / proj[0][0],
			( 1.0f - proj[1][2] ) / proj[1][1]
		};
		configuration.numSamples = config.numSamples;
		configuration.numSpiralTurns = numSpiralTurns;
		configuration.projScale = projScale;
		configuration.radius = radius;
		configuration.invRadius = invRadius;
		configuration.radius2 = radius2;
		configuration.invRadius2 = invRadius2;
		configuration.bias = config.bias;
		configuration.intensity = config.intensity;
		configuration.intensityDivR6 = intersityDivR6;
		configuration.farPlaneZ = farZ;
		configuration.edgeSharpness = config.edgeSharpness;
		configuration.blurStepSize = config.blurStepSize.value().value();
		configuration.blurRadius = config.blurRadius.value().value();
		configuration.blurHighQuality = config.blurHighQuality ? 1u : 0u;
		configuration.highQuality = config.highQuality ? 1u : 0u;
		configuration.logMaxOffset = config.logMaxOffset.value();
		configuration.maxMipLevel = config.maxMipLevel;
		configuration.minRadius = config.minRadius;
		configuration.variation = config.variation;
		configuration.bendStepCount = config.bendStepCount.value();
		configuration.bendStepSize = config.bendStepSize;
	}

	//*********************************************************************************************
}
