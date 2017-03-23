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
#ifndef ___C3DGLES3_SAMPLER_H___
#define ___C3DGLES3_SAMPLER_H___

#include "Common/GlES3Object.hpp"

#include <Texture/Sampler.hpp>

namespace GlES3Render
{
	class GlES3Sampler
		: public Castor3D::Sampler
		, private Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< void( int, uint32_t * ) >, std::function< void( int, uint32_t const * ) > >;

	private:
		typedef std::function< void() > PUnbindFunction;
		typedef std::function< void( uint32_t p_index ) > PBindFunction;

	public:
		GlES3Sampler( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, Castor::String const & p_name );
		virtual ~GlES3Sampler();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual void Bind( uint32_t p_index )const;
		virtual void Unbind( uint32_t p_index )const;

	private:
		void DoAdjustMinMipModes( GlES3InterpolationMode & p_min, GlES3InterpolationMode & p_mip );

	private:
		GlES3TexDim m_glDimension;
	};
}

#endif
