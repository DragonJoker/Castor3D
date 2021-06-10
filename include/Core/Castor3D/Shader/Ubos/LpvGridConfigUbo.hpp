/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvGridConfigUbo_H___
#define ___C3D_LpvGridConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct LpvGridData
			: public sdw::StructInstance
		{
			C3D_API LpvGridData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API LpvGridData & operator=( LpvGridData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			sdw::Vec4 minVolumeCornerSize;
			sdw::Vec4 gridSizeAtt;
			// Specific values
			sdw::Vec3 minVolumeCorner;
			sdw::Float cellSize;
			sdw::Float gridWidth;
			sdw::Float gridHeight;
			sdw::Float gridDepth;
			sdw::Vec3 gridSize;
			sdw::Float indirectAttenuation;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class LpvGridConfigUbo
	{
	public:
		using Configuration = LpvGridConfigUboConfiguration;

	public:
		C3D_API explicit LpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LpvGridConfigUbo();

		C3D_API void cpuUpdate( castor::BoundingBox const & aabb
			, castor::Point3f const & cameraPos
			, castor::Point3f const & cameraDir
			, uint32_t gridDim
			, float indirectAttenuation );
		C3D_API castor::Grid cpuUpdate( uint32_t gridLevel
			, float gridLevelScale
			, castor::Grid const & grid
			, castor::BoundingBox const & aabb
			, castor::Point3f const & cameraPos
			, castor::Point3f const & cameraDir
			, float indirectAttenuation );

		UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "LpvGridCfg", binding );
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

	public:
		C3D_API static const std::string LpvGridConfig;
		C3D_API static const std::string LpvGridData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_LPVGRIDCONFIG( writer, binding, set, enabled )\
	sdw::Ubo lpvGridConfig{ writer\
		, castor3d::LpvGridConfigUbo::LpvGridConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lpvGridData = lpvGridConfig.declStructMember< castor3d::shader::LpvGridData >( castor3d::LpvGridConfigUbo::LpvGridData, enabled );\
	lpvGridConfig.end()

#endif
