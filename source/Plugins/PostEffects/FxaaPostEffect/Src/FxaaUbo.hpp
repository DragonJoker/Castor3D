/*
See LICENSE file in root folder
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace fxaa
{
	class FxaaUbo
	{
	public:
		explicit FxaaUbo( castor3d::Engine & engine );
		~FxaaUbo();
		void update( castor::Size const & p_size
			, float p_shift
			, float p_span
			, float p_reduce );

		inline castor3d::UniformBuffer & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::String Name;
		static const castor::String SubpixShift;
		static const castor::String SpanMax;
		static const castor::String ReduceMul;
		static const castor::String RenderSize;
		static constexpr uint32_t BindingPoint = 2u;

	private:
		castor3d::UniformBuffer m_ubo;
		castor3d::Uniform1fSPtr m_subpixShift;
		castor3d::Uniform1fSPtr m_spanMax;
		castor3d::Uniform1fSPtr m_reduceMul;
		castor3d::Uniform2fSPtr m_renderSize;
	};
}

#define UBO_FXAA( p_writer )\
	glsl::Ubo fxaa{ p_writer, FxaaUbo::Name, FxaaUbo::BindingPoint };\
	auto c3d_subpixShift = fxaa.declMember< Float >( FxaaUbo::SubpixShift );\
	auto c3d_spanMax = fxaa.declMember< Float >( FxaaUbo::SpanMax );\
	auto c3d_reduceMul = fxaa.declMember< Float >( FxaaUbo::ReduceMul );\
	auto c3d_renderSize = fxaa.declMember< Vec2 >( FxaaUbo::RenderSize );\
	fxaa.end()

#endif
