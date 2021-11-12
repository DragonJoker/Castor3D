#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPools.hpp"
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
			, gridConv{ getMember< sdw::Vec4 >( "gridConv" ) }
			, radiance{ getMember< sdw::Vec4 >( "radiance" ) }
			, other{ getMember< sdw::Vec4 >( "other" ) }
			, status{ getMember< sdw::UVec4 >( "status" ) }
			, worldToGrid{ gridConv.x() }
			, gridToWorld{ gridConv.y() }
			, clipToGrid{ gridConv.z() }
			, gridToClip{ gridConv.w() }
			, radianceMaxDistance{ radiance.x() }
			, radianceMips{ radiance.y() }
			, radianceNumCones{ writer.cast< sdw::UInt >( radiance.z() ) }
			, radianceNumConesInv{ radiance.w() }
			, gridCenter{ other.xyz() }
			, rayStepSize{ other.w() }
			, enabled{ status.x() }
			, enableConservativeRasterization{ status.y() }
			, enableOcclusion{ status.z() }
			, enableSecondaryBounce{ status.w() }
		{
		}

		ast::type::BaseStructPtr VoxelData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_VoxelData" );

			if ( result->empty() )
			{
				result->declMember( "gridConv", ast::type::Kind::eVec4F );
				result->declMember( "radiance", ast::type::Kind::eVec4F );
				result->declMember( "other", ast::type::Kind::eVec4F );
				result->declMember( "status", ast::type::Kind::eVec4U );
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

	castor::String const VoxelizerUbo::BufferVoxelizer = cuT( "Voxelizer" );
	castor::String const VoxelizerUbo::VoxelData = cuT( "c3d_voxelData" );

	VoxelizerUbo::VoxelizerUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< Configuration >( 0u ) }
	{
	}

	VoxelizerUbo::~VoxelizerUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void VoxelizerUbo::cpuUpdate( VoxelSceneData const & voxelConfig
		, float worldToGrid
		, uint32_t voxelGridSize )
	{
		CU_Require( m_ubo );
		auto & voxelData = m_ubo.getData();
		voxelData.gridConv->x = worldToGrid;
		voxelData.gridConv->y = 1.0f / worldToGrid;
		voxelData.gridConv->z = float( voxelGridSize );
		voxelData.gridConv->w = 1.0f / float( voxelGridSize );
		voxelData.radiance->x = voxelConfig.maxDistance;
		voxelData.radiance->y = float( castor::getBitSize( voxelGridSize ) );
		voxelData.radiance->z = float( voxelConfig.numCones.value() );
		voxelData.radiance->w = 1.0f / voxelData.radiance->z;
		voxelData.other->w = voxelConfig.rayStepSize;
		voxelData.status->x = voxelConfig.enabled ? 1u : 0u;
		voxelData.status->y = voxelConfig.enableConservativeRasterization ? 1u : 0u;
		voxelData.status->z = voxelConfig.enableOcclusion ? 1u : 0u;
		voxelData.status->w = voxelConfig.enableSecondaryBounce ? 1u : 0u;
	}

	//*********************************************************************************************
}
