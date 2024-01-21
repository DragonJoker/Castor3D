/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizerUbo_H___
#define ___C3D_VoxelizerUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"

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
			SDW_DeclStructInstance( C3D_API, VoxelData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 worldToTex( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::IVec3 worldToImg( sdw::Vec3 const & wsPosition )const;
			C3D_API sdw::Vec3 worldToClip( sdw::Vec3 const & wsPosition )const;

			// Raw values
			sdw::Float worldToGrid;
			sdw::Float gridToWorld;
			sdw::Float clipToGrid;
			sdw::Float gridToClip;

			sdw::Float radianceMaxDistance;
			sdw::Float radianceMips;
			sdw::UInt radianceNumCones;
			sdw::Float radianceNumConesInv;

			//! sdw::Vec3 pad;
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
		float worldToGrid;
		float gridToWorld;
		float clipToGrid;
		float gridToClip;
		/**
		*	- float radianceMaxDistance;
		*	- float radianceMips;
		*	- uint radianceNumCones;
		*	- float radianceNumConesInv;
		*/
		float radianceMaxDistance;
		float radianceMips;
		uint32_t radianceNumCones;
		float radianceNumConesInv;
		/**
		*	- vec3 gridCenter: Center of the voxel grid, in world space units
		*	- float rayStepSize;
		*/
		castor::Point3f pad;
		float rayStepSize;
		/**
		*	- uint enabled;
		*	- uint enableConservativeRasterization;
		*	- uint enableOcclusion;
		*	- uint enableSecondaryBounce;
		*/
		uint32_t enabled;
		uint32_t enableConservativeRasterization;
		uint32_t enableOcclusion;
		uint32_t enableSecondaryBounce;
	};

	class VoxelizerUbo
	{
	public:
		using Configuration = VoxelizerUboConfiguration;

	public:
		C3D_API VoxelizerUbo( VoxelizerUbo const & rhs ) = delete;
		C3D_API VoxelizerUbo & operator=( VoxelizerUbo const & rhs ) = delete;
		C3D_API VoxelizerUbo( VoxelizerUbo && rhs )noexcept = default;
		C3D_API VoxelizerUbo & operator=( VoxelizerUbo && rhs )noexcept = delete;
		C3D_API explicit VoxelizerUbo( RenderDevice const & device );
		C3D_API ~VoxelizerUbo()noexcept;

		C3D_API void cpuUpdate( VctConfig const & voxelConfig
			, float worldToGrid
			, uint32_t voxelGridSize );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "VoxelCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			descriptorWrites.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}


	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo{};
	};
}

#define C3D_Voxelizer( writer, binding, set, enable )\
	sdw::UniformBuffer voxelizer{ writer\
		, "C3D_Voxelizer"\
		, "c3d_voxelizer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, enable };\
	auto c3d_voxelData = voxelizer.declMember< shader::VoxelData >( "c3d_voxelData" );\
	voxelizer.end()

#endif
