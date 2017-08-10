/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DFXAA_FxaaUbo_H___
#define ___C3DFXAA_FxaaUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace fxaa
{
	class FxaaUbo
	{
	public:
		FxaaUbo( castor3d::Engine & engine );
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
	GLSL::Ubo fxaa{ p_writer, FxaaUbo::Name, FxaaUbo::BindingPoint };\
	auto c3d_subpixShift = fxaa.declMember< Float >( FxaaUbo::SubpixShift );\
	auto c3d_spanMax = fxaa.declMember< Float >( FxaaUbo::SpanMax );\
	auto c3d_reduceMul = fxaa.declMember< Float >( FxaaUbo::ReduceMul );\
	auto c3d_renderSize = fxaa.declMember< Vec2 >( FxaaUbo::RenderSize );\
	fxaa.end()

#endif
