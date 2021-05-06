/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizerUbo_H___
#define ___C3D_VoxelizerUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelizeModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct VoxelData
			: public sdw::StructInstance
		{
			C3D_API VoxelData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API VoxelData & operator=( VoxelData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 worldToTex( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::IVec3 worldToImg( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::Vec3 worldToClip( sdw::Vec3 const & wsPosition )const;

			// Raw values
			sdw::Vec4 gridConv;
			sdw::Vec4 radiance;
			sdw::Vec4 other;
			sdw::UVec4 status;
			// Specific values
			sdw::Float worldToGrid;
			sdw::Float gridToWorld;
			sdw::Float clipToGrid;
			sdw::Float gridToClip;
			sdw::Float radianceMaxDistance;
			sdw::Float radianceMips;
			sdw::UInt radianceNumCones;
			sdw::Float radianceNumConesInv;
			sdw::Vec3 gridCenter;
			sdw::Float rayStepSize;
			sdw::UInt enabled;
			sdw::UInt enableConservativeRasterization;
			sdw::UInt enableOcclusion;
			sdw::UInt enableSecondaryBounce;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	struct VoxelizerUboConfiguration
	{
		/**
		*	- float worldToGrid: Factor from world position to grid position
		*	- float gridToWorld: Factor from grid position to world position
		*	- float gridToClip: Factor from grid position to clip position
		*	- float clipToGrid: Factor from clip position to grid position
		*/
		castor::Point4f gridConv;
		/**
		*	- float radianceMaxDistance;
		*	- float radianceMips;
		*	- uint radianceNumCones;
		*	- float radianceNumConesInv;
		*/
		castor::Point4f radiance;
		/**
		*	- vec3 gridCenter: Center of the voxel grid, in world space units
		*	- float rayStepSize;
		*/
		castor::Point4f other;
		/**
		*	- uint enabled;
		*	- uint conservativeRasterization;
		*	- uint enableOcclusion;
		*	- uint enableSecondaryBounce;
		*/
		castor::Point4ui status;
	};

	class VoxelizerUbo
	{
	public:
		using Configuration = VoxelizerUboConfiguration;

	public:
		C3D_API explicit VoxelizerUbo( RenderDevice const & device );
		C3D_API ~VoxelizerUbo();

		C3D_API void cpuUpdate( VoxelSceneData const & voxelConfig
			, float worldToGrid
			, uint32_t voxelGridSize );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static castor::String const BufferVoxelizer;
		C3D_API static castor::String const VoxelData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo{};
	};
}

#define UBO_VOXELIZER( writer, binding, set, enable )\
	sdw::Ubo voxelizer{ writer\
		, castor3d::VoxelizerUbo::BufferVoxelizer\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, enable };\
	auto c3d_voxelData = voxelizer.declStructMember< shader::VoxelData >( castor3d::VoxelizerUbo::VoxelData );\
	voxelizer.end()

#endif
