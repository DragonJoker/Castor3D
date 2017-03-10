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
#ifndef ___C3D_PointLightPass_H___
#define ___C3D_PointLightPass_H___

#include "LightPass.hpp"

namespace deferred
{
	class PointLightPass
		: public LightPass
	{
	public:
		PointLightPass( Castor3D::Engine & p_engine
			, Castor3D::UniformBuffer & p_matrixUbo
			, Castor3D::UniformBuffer & p_sceneUbo );
		void Create( Castor3D::ShaderProgramSPtr p_program
			, Castor3D::Scene const & p_scene );
		void Destroy();
		void Initialise();
		void Cleanup();
		void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::PointLight const & p_light
			, bool p_first );

	private:
		//!\~english	The variable containing the light position.
		//!\~french		La variable contenant la position de la lumière.
		Castor3D::PushUniform3fSPtr m_lightPosition;
		//!\~english	The variable containing the light attenuation.
		//!\~french		La variable contenant l'atténuation de la lumière.
		Castor3D::PushUniform3fSPtr m_lightAttenuation;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		Castor3D::BufferDeclaration m_declaration;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
	};
	using PointLightPassPrograms = std::array< PointLightPass, size_t( GLSL::FogType::eCount ) >;
}

#endif
