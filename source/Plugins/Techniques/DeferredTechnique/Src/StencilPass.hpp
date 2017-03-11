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
#ifndef ___C3D_StencilPass_H___
#define ___C3D_StencilPass_H___

#include "LightPass.hpp"

namespace deferred
{
	class StencilPass
	{
	public:
		void Initialise( Castor3D::VertexBuffer & p_vbo
			, Castor3D::IndexBufferSPtr p_ibo );
		void Cleanup();
		void Render( uint32_t p_count );

	private:
		//!\~english	The shader program used to render lights.
		//!\~french		Le shader utilisé pour rendre les lumières.
		Castor3D::ShaderProgramSPtr m_program;
		//!\~english	Geometry buffers holder.
		//!\~french		Conteneur de buffers de géométries.
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		Castor3D::RenderPipelineSPtr m_pipeline;
	};
}

#endif
