#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/Writer.hpp>

CU_ImplementCUSmartPtr( castor3d, LpvLightConfigUbo )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		castor::Matrix4x4f snapMatrix( float lpvCellSize
			, castor::Matrix4x4f mtx )
		{
			mtx[0][3] = mtx[0][3] - fmod( mtx[0][3], lpvCellSize );
			mtx[1][3] = mtx[1][3] - fmod( mtx[1][3], lpvCellSize );
			mtx[2][3] = mtx[2][3] - fmod( mtx[2][3], lpvCellSize );
			return mtx;
		}
	}

	//*********************************************************************************************

	namespace shader
	{
		LpvLightData::LpvLightData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, lightView{ getMember< sdw::Mat4 >( "lightView" ) }
			, lightConfig{ getMember< sdw::Vec4 >( "lightConfig" ) }
			, texelAreaModifier{ lightConfig.x() }
			, tanFovXHalf{ lightConfig.y() }
			, tanFovYHalf{ lightConfig.z() }
			, lightIndex{ writer.cast< sdw::Int >( lightConfig.w() ) }
		{
		}

		LpvLightData & LpvLightData::operator=( LpvLightData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr LpvLightData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "LpvLightData" );

			if ( result->empty() )
			{
				result->declMember( "lightView", ast::type::Kind::eMat4x4F );
				result->declMember( "lightConfig", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > LpvLightData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	std::string const LpvLightConfigUbo::LpvLightConfig = "LpvLightConfig";
	std::string const LpvLightConfigUbo::LpvLightData = "c3d_lpvLightData";

	LpvLightConfigUbo::LpvLightConfigUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}
	
	LpvLightConfigUbo::~LpvLightConfigUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void LpvLightConfigUbo::cpuUpdate( Light const & light
		, float lpvCellSize
		, uint32_t faceIndex )
	{
		CU_Require( m_ubo );
		auto & lpvConfig = light.getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightIndex = float( light.getBufferIndex() );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		auto ltType = light.getLightType();

		switch ( ltType )
		{
		case LightType::eDirectional:
			CU_Require( faceIndex == 0u );
			configuration.lightView = snapMatrix( lpvCellSize
				, light.getDirectionalLight()->getViewMatrix( shader::DirectionalMaxCascadesCount - 1u ) );
			configuration.tanFovXHalf = 1.0f;
			configuration.tanFovYHalf = 1.0f;
			break;

		case LightType::ePoint:
			{
				CU_Require( faceIndex < 6u );
				auto lightFov = 90.0_degrees;
				auto & pointLight = *light.getPointLight();
				configuration.lightView = snapMatrix( lpvCellSize
					, pointLight.getViewMatrix( CubeMapFace( faceIndex ) ) );
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;

		case LightType::eSpot:
			{
				CU_Require( faceIndex == 0u );
				auto & spotLight = *light.getSpotLight();
				configuration.lightView = snapMatrix( lpvCellSize
					, spotLight.getViewMatrix() );
				auto lightFov = spotLight.getCutOff();
				configuration.tanFovXHalf = ( lightFov * 0.5 ).tan();
				configuration.tanFovYHalf = ( lightFov * 0.5 ).tan();
			}
			break;
		}
	}

	void LpvLightConfigUbo::cpuUpdate( DirectionalLight const & light
		, uint32_t cascadeIndex
		, float lpvCellSize )
	{
		auto & lpvConfig = light.getLight().getLpvConfig();
		auto & configuration = m_ubo.getData();

		configuration.lightView = snapMatrix( lpvCellSize
			, light.getViewMatrix( cascadeIndex ) );
		configuration.texelAreaModifier = lpvConfig.texelAreaModifier;
		configuration.tanFovXHalf = 1.0f;
		configuration.tanFovYHalf = 1.0f;
		configuration.lightIndex = float( light.getLight().getBufferIndex() );
	}
}
