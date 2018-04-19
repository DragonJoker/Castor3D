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
			castor::Point2f pixelSize;
			float subpixShift;
			float spanMax;
			float reduceMul;
		};

	public:
		explicit FxaaUbo( castor3d::Engine & engine
			, castor::Size const & size );
		void update( float shift
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
		static const castor::String PixelSize;

	private:
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_FXAA( writer, binding, set )\
	glsl::Ubo fxaa{ writer, FxaaUbo::Name, binding, set };\
	auto c3d_pixelSize = fxaa.declMember< Vec2 >( FxaaUbo::PixelSize );\
	auto c3d_subpixShift = fxaa.declMember< Float >( FxaaUbo::SubpixShift );\
	auto c3d_spanMax = fxaa.declMember< Float >( FxaaUbo::SpanMax );\
	auto c3d_reduceMul = fxaa.declMember< Float >( FxaaUbo::ReduceMul );\
	fxaa.end()

#endif
