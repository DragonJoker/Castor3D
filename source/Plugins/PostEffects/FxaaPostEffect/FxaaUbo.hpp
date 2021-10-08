/*
See LICENSE file in root folder
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace fxaa
{
	struct FxaaUboConfiguration
	{
		castor::Point2f pixelSize;
		float subpixShift;
		float spanMax;
		float reduceMul;
	};

	struct FxaaData
		: public sdw::StructInstance
	{
	public:
		FxaaData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( , FxaaData );

		static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

	public:
		sdw::Vec2 pixelSize;
		sdw::Float subpixShift;
		sdw::Float spanMax;
		sdw::Float reduceMul;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	class FxaaUbo
	{
	private:
		using Configuration = FxaaUboConfiguration;

	public:
		explicit FxaaUbo( castor3d::RenderDevice const & device
			, castor::Size const & size );
		~FxaaUbo();
		void cpuUpdate( float shift
			, float span
			, float reduce );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "FxaaCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		static const castor::String Buffer;
		static const castor::String Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_FXAA( writer, binding, set )\
	sdw::Ubo fxaa{ writer, FxaaUbo::Buffer, binding, set };\
	auto c3d_fxaaData = fxaa.declStructMember< FxaaData >( FxaaUbo::Data );\
	fxaa.end()

#endif
