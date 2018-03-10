/*
See LICENSE file in root folder
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include <Castor3DPrerequisites.hpp>
#include <Buffer/UniformBuffer.hpp>

namespace fxaa
{
	class FxaaUbo
	{
	private:
		struct Configuration
		{
			float subpixShift;
			float spanMax;
			float reduceMul;
			castor::Point2f renderSize;
		};

	public:
		explicit FxaaUbo( castor3d::Engine & engine );
		void update( castor::Size const & size
			, float shift
			, float span
			, float reduce );

		inline renderer::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

	public:
		static const castor::String Name;
		static const castor::String SubpixShift;
		static const castor::String SpanMax;
		static const castor::String ReduceMul;
		static const castor::String RenderSize;
		static constexpr uint32_t BindingPoint = 2u;

	private:
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_FXAA( writer, set )\
	glsl::Ubo fxaa{ writer, FxaaUbo::Name, FxaaUbo::BindingPoint, set };\
	auto c3d_subpixShift = fxaa.declMember< Float >( FxaaUbo::SubpixShift );\
	auto c3d_spanMax = fxaa.declMember< Float >( FxaaUbo::SpanMax );\
	auto c3d_reduceMul = fxaa.declMember< Float >( FxaaUbo::ReduceMul );\
	auto c3d_renderSize = fxaa.declMember< Vec2 >( FxaaUbo::RenderSize );\
	fxaa.end()

#endif
