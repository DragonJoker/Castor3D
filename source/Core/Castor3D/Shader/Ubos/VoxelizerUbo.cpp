#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		VoxelData::VoxelData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, worldToGrid{ getMember< sdw::Float >( "worldToGrid" ) }
			, gridToWorld{ getMember< sdw::Float >( "gridToWorld" ) }
			, clipToGrid{ getMember< sdw::Float >( "clipToGrid" ) }
			, gridToClip{ getMember< sdw::Float >( "gridToClip" ) }
			, radianceMaxDistance{ getMember< sdw::Float >( "radianceMaxDistance" ) }
			, radianceMips{ getMember< sdw::Float >( "radianceMips" ) }
			, radianceNumCones{ getMember< sdw::UInt >( "radianceNumCones" ) }
			, radianceNumConesInv{ getMember< sdw::Float >( "radianceNumConesInv" ) }
			//, pad{ getMember< sdw::Vec3 >( "pad" ) }
			, rayStepSize{ getMember< sdw::Float >( "rayStepSize" ) }
			, enabled{ getMember< sdw::UInt >( "enabled" ) }
			, enableConservativeRasterization{ getMember< sdw::UInt >( "enableConservativeRasterization" ) }
			, enableOcclusion{ getMember< sdw::UInt >( "enableOcclusion" ) }
			, enableSecondaryBounce{ getMember< sdw::UInt >( "enableSecondaryBounce" ) }
		{
		}

		ast::type::BaseStructPtr VoxelData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_VoxelData" );

			if ( result->empty() )
			{
				result->declMember( "worldToGrid", ast::type::Kind::eFloat );
				result->declMember( "gridToWorld", ast::type::Kind::eFloat );
				result->declMember( "clipToGrid", ast::type::Kind::eFloat );
				result->declMember( "gridToClip", ast::type::Kind::eFloat );

				result->declMember( "radianceMaxDistance", ast::type::Kind::eFloat );
				result->declMember( "radianceMips", ast::type::Kind::eFloat );
				result->declMember( "radianceNumCones", ast::type::Kind::eUInt32 );
				result->declMember( "radianceNumConesInv", ast::type::Kind::eFloat );

				result->declMember( "pad", ast::type::Kind::eVec3F );
				result->declMember( "rayStepSize", ast::type::Kind::eFloat );

				result->declMember( "enabled", ast::type::Kind::eUInt32 );
				result->declMember( "enableConservativeRasterization", ast::type::Kind::eUInt32 );
				result->declMember( "enableOcclusion", ast::type::Kind::eUInt32 );
				result->declMember( "enableSecondaryBounce", ast::type::Kind::eUInt32 );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > VoxelData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec3 VoxelData::worldToTex( sdw::Vec3 const & wsPosition )const
		{
			return worldToClip( wsPosition ) * vec3( 0.5_f, -0.5_f, 0.5_f ) + vec3( 0.5_f );
		}

		sdw::IVec3 VoxelData::worldToImg( sdw::Vec3 const & wsPosition )const
		{
			return ivec3( worldToTex( wsPosition ) * clipToGrid );
		}

		sdw::Vec3 VoxelData::worldToClip( sdw::Vec3 const & wsPosition )const
		{
			return wsPosition * worldToGrid * gridToClip;
		}
	}

	//*********************************************************************************************

	VoxelizerUbo::VoxelizerUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	VoxelizerUbo::~VoxelizerUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void VoxelizerUbo::cpuUpdate( VoxelSceneData const & voxelConfig
		, float worldToGrid
		, uint32_t voxelGridSize )
	{
		CU_Require( m_ubo );
		auto & voxelData = m_ubo.getData();
		voxelData.worldToGrid = worldToGrid;
		voxelData.gridToWorld = 1.0f / worldToGrid;
		voxelData.clipToGrid = float( voxelGridSize );
		voxelData.gridToClip = 1.0f / float( voxelGridSize );
		voxelData.radianceMaxDistance = voxelConfig.maxDistance;
		voxelData.radianceMips = float( castor::getBitSize( voxelGridSize ) );
		voxelData.radianceNumCones = voxelConfig.numCones.value();
		voxelData.radianceNumConesInv = 1.0f / float( voxelData.radianceNumCones );
		voxelData.rayStepSize = voxelConfig.rayStepSize;
		voxelData.enabled = voxelConfig.enabled ? 1u : 0u;
		voxelData.enableConservativeRasterization = voxelConfig.enableConservativeRasterization ? 1u : 0u;
		voxelData.enableOcclusion = voxelConfig.enableOcclusion ? 1u : 0u;
		voxelData.enableSecondaryBounce = voxelConfig.enableSecondaryBounce ? 1u : 0u;
	}

	//*********************************************************************************************
}
