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
#ifndef ___GLT_GlTextureTest_H___
#define ___GLT_GlTextureTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <Design/ArrayView.hpp>

#include <cstring>

namespace Testing
{
	class GlTextureTest
		: public GlTestCase
	{
	public:
		explicit GlTextureTest( Castor3D::Engine & p_engine );
		virtual ~GlTextureTest();

		void Upload( Castor3D::TextureLayout & p_storage, Castor::ArrayView< uint8_t > const & p_view );
		void Download( Castor3D::TextureLayout & p_storage, std::vector< uint8_t > & p_dst );
		void Compare( std::array< uint8_t, 8 * 8 * 3 > const & p_src, std::vector< uint8_t > const & p_dst );

	private:
		void DoRegisterTests()override;

	private:
		void ImmutableStorage();
		void DirectStorage();
		void PboStorage();
		void GpuOnlyStorage();
		void TboStorage();
	};
}

#endif
