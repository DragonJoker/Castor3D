#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"
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
			, sizeResolution{ getMember< sdw::Vec4 >( "sizeResolution" ) }
			, radiance{ getMember< sdw::Vec4 >( "radiance" ) }
			, other{ getMember< sdw::Vec4 >( "other" ) }
			, size{ sizeResolution.x() }
			, sizeInv{ sizeResolution.y() }
			, resolution{ sizeResolution.z() }
			, resolutionInv{ sizeResolution.w() }
			, radianceMaxDistance{ radiance.x() }
			, radianceMips{ radiance.y() }
			, radianceNumCones{ writer.cast< sdw::UInt >( radiance.z() ) }
			, radianceNumConesInv{ radiance.w() }
			, rayStepSize{ other.w() }
			, center{ other.xyz() }
		{
		}

		VoxelData & VoxelData::operator=( VoxelData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr VoxelData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "VoxelData" );

			if ( result->empty() )
			{
				result->declMember( "sizeResolution", ast::type::Kind::eVec4F );
				result->declMember( "radiance", ast::type::Kind::eVec4F );
				result->declMember( "other", ast::type::Kind::eVec4F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > VoxelData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}
	}

	//*********************************************************************************************

	uint32_t const VoxelizerUbo::BindingPoint = 13u;
	castor::String const VoxelizerUbo::BufferVoxelizer = cuT( "Voxelizer" );
	castor::String const VoxelizerUbo::VoxelData = cuT( "c3d_voxelData" );

	VoxelizerUbo::VoxelizerUbo()
	{
	}

	VoxelizerUbo::VoxelizerUbo( RenderDevice const & device )
	{
		initialise( device );
	}

	VoxelizerUbo::~VoxelizerUbo()
	{
		cleanup();
	}

	void VoxelizerUbo::initialise( RenderDevice const & device )
	{
		m_device = &device;
		m_ubo = m_device->uboPools->getBuffer< Configuration >( 0u );
	}

	void VoxelizerUbo::cleanup()
	{
		if ( m_ubo )
		{
			m_device->uboPools->putBuffer( m_ubo );
			m_device = nullptr;
			m_ubo = {};
		}
	}

	void VoxelizerUbo::cpuUpdate( VoxelSceneData const & voxelConfig
		, castor::Point3f const & center
		, uint32_t voxelGridSize )
	{
		CU_Require( m_ubo );
		auto & voxelData = m_ubo.getData();
		voxelData.sizeResolution->x = voxelConfig.voxelSize;
		voxelData.sizeResolution->y = 1.0f / voxelData.sizeResolution->x;
		voxelData.sizeResolution->z = float( voxelGridSize );
		voxelData.sizeResolution->w = 1.0f / voxelData.sizeResolution->z;
		voxelData.radiance->x = voxelConfig.maxDistance;
		voxelData.radiance->y = uint32_t( castor::getBitSize( voxelGridSize ) );
		voxelData.radiance->z = voxelConfig.numCones.value();
		voxelData.radiance->w = 1.0f / voxelData.radiance->z;
		voxelData.other->x = center->x;
		voxelData.other->y = center->y;
		voxelData.other->z = center->z;
		voxelData.other->w = voxelConfig.rayStepSize;
	}

	//*********************************************************************************************
}
