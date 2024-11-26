#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
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

	VoxelizerUbo::~VoxelizerUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void VoxelizerUbo::cpuUpdate( VctConfig const & voxelConfig
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
