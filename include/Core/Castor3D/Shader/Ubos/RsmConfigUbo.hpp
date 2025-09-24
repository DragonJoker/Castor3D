/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfigUbo_H___
#define ___C3D_RsmConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapsModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct RsmConfigData
			: public sdw::StructInstanceHelperT< "C3D_RsmData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::FloatField< "intensity" >
				, sdw::FloatField< "maxRadius" >
				, sdw::UIntField< "sampleCount" >
				, sdw::IntField< "index" > >
		{
			SDW_DeclStructInstance( C3D_API, RsmConfigData );

			RsmConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, intensity{ getMember< "intensity" >() }
				, maxRadius{ getMember< "maxRadius" >() }
				, sampleCount{ getMember< "sampleCount" >() }
				, index{ getMember< "index" >() }
			{
			}

			sdw::Float intensity;
			sdw::Float maxRadius;
			sdw::UInt32 sampleCount;
			sdw::Int32 index;
		};
	}

	class RsmConfigUbo
	{
	public:
		using Configuration = RsmUboConfiguration;

	public:
		C3D_API RsmConfigUbo( RsmConfigUbo const & rhs ) = delete;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo const & rhs ) = delete;
		C3D_API RsmConfigUbo( RsmConfigUbo && rhs )noexcept = delete;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo && rhs )noexcept = delete;
		C3D_API explicit RsmConfigUbo( RenderDevice const & device );
		C3D_API ~RsmConfigUbo()noexcept;

		C3D_API void cpuUpdate( RsmConfig const & rsmConfig
			, uint32_t index );

		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding )const
		{
			m_ubo.createPassBinding( pass, binding );
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

#define C3D_RsmConfigEx( writer, binding, set, enable )\
	sdw::UniformBuffer rsmConfig{ writer\
		, "C3D_RsmConfig"\
		, "c3d_rsmConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, enable };\
	auto c3d_rsmConfigData = rsmConfig.declMember< c3d::shader::RsmConfigData >( "c3d_rsmConfigData" );\
	rsmConfig.end()

#define C3D_RsmConfig( writer, binding, set )\
	C3D_RsmConfigEx( writer, binding, set, true )

#endif
