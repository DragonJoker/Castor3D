/*
See LICENSE file in root folder
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

namespace fxaa
{
	class FxaaUbo
	{
	private:
		struct Configuration
		{
			castor::Point2f pixelSize;
			float subpixShift;
			float spanMax;
			float reduceMul;
		};

	public:
		explicit FxaaUbo( castor3d::Engine & engine
			, castor::Size const & size );
		~FxaaUbo();
		void cpuUpdate( float shift
			, float span
			, float reduce );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

	public:
		static const castor::String Name;
		static const castor::String SubpixShift;
		static const castor::String SpanMax;
		static const castor::String ReduceMul;
		static const castor::String PixelSize;

	private:
		castor3d::Engine & m_engine;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_FXAA( writer, binding, set )\
	sdw::Ubo fxaa{ writer, FxaaUbo::Name, binding, set };\
	auto c3d_pixelSize = fxaa.declMember< Vec2 >( FxaaUbo::PixelSize );\
	auto c3d_subpixShift = fxaa.declMember< Float >( FxaaUbo::SubpixShift );\
	auto c3d_spanMax = fxaa.declMember< Float >( FxaaUbo::SpanMax );\
	auto c3d_reduceMul = fxaa.declMember< Float >( FxaaUbo::ReduceMul );\
	fxaa.end()

#endif
