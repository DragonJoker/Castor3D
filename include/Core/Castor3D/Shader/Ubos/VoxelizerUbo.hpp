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

			// Raw values
			sdw::Vec4 sizeResolution;
			sdw::Vec4 radiance;
			sdw::Vec4 other;
			// Specific values
			sdw::Float size;
			sdw::Float sizeInv;
			sdw::Float resolution;
			sdw::Float resolutionInv;
			sdw::Float radianceMaxDistance;
			sdw::Float radianceMips;
			sdw::UInt radianceNumCones;
			sdw::Float radianceNumConesInv;
			sdw::Float rayStepSize;
			sdw::Vec3 center;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class VoxelizerUbo
	{
	public:
		using Configuration = VoxelizerUboConfiguration;

	public:
		C3D_API VoxelizerUbo( VoxelizerUbo const & ) = delete;
		C3D_API VoxelizerUbo & operator=( VoxelizerUbo const & ) = delete;
		C3D_API VoxelizerUbo( VoxelizerUbo && ) = default;
		C3D_API VoxelizerUbo & operator=( VoxelizerUbo && ) = delete;

		C3D_API explicit VoxelizerUbo();
		C3D_API explicit VoxelizerUbo( RenderDevice const & device );
		C3D_API ~VoxelizerUbo();

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		C3D_API void cpuUpdate( VoxelSceneData const & voxelConfig
			, castor::Point3f const & center
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
		C3D_API static uint32_t const BindingPoint;
		C3D_API static castor::String const BufferVoxelizer;
		C3D_API static castor::String const VoxelData;

	private:
		RenderDevice const * m_device{};
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
