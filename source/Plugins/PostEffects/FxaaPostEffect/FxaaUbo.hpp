/*
See LICENSE file in root folder
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace fxaa
{
	struct FxaaUboConfiguration
	{
		c3d::Point2f pixelSize;
		float subpixShift;
		float spanMax;
		float reduceMul;
	};

	struct FxaaData
		: public sdw::StructInstanceHelperT< "C3D_FxaaData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec2Field< "pixelSize" >
			, sdw::FloatField< "subpixShift" >
			, sdw::FloatField< "spanMax" >
			, sdw::FloatField< "reduceMul" > >
	{
		SDW_DeclStructInstance( , FxaaData );

		FxaaData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, pixelSize{ getMember< "pixelSize" >() }
			, subpixShift{ getMember< "subpixShift" >() }
			, spanMax{ getMember< "spanMax" >() }
			, reduceMul{ getMember< "reduceMul" >() }
		{
		}

		sdw::Vec2 pixelSize;
		sdw::Float subpixShift;
		sdw::Float spanMax;
		sdw::Float reduceMul;
	};

	class FxaaUbo
	{
	private:
		using Configuration = FxaaUboConfiguration;

	public:
		explicit FxaaUbo( c3d::RenderDevice const & device
			, c3d::Size const & size );
		~FxaaUbo();
		void cpuUpdate( float shift
			, float span
			, float reduce );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		c3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Fxaa( writer, binding, set )\
	sdw::UniformBuffer fxaa{ writer, FxaaUbo::Buffer, binding, set };\
	auto c3d_fxaaData = fxaa.declMember< FxaaData >( FxaaUbo::Data );\
	fxaa.end()

#endif
