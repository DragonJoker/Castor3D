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
#ifndef ___GL_SAMPLER_H___
#define ___GL_SAMPLER_H___

#include "Common/GlObject.hpp"

#include <Texture/Sampler.hpp>

namespace GlRender
{
	class GlSampler
		: public Castor3D::Sampler
		, private Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< bool( uint32_t p_index ) > PBindFunction;

	public:
		GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name );
		virtual ~GlSampler();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Bind( uint32_t p_index )const;
		virtual void Unbind( uint32_t p_index )const;

	private:
		void DoAdjustMinMipModes( eGL_INTERPOLATION_MODE & p_min, eGL_INTERPOLATION_MODE & p_mip );

	private:
		eGL_TEXDIM m_glDimension;
	};
}

#endif
