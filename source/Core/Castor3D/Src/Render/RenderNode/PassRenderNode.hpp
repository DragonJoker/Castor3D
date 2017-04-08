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
#ifndef ___C3D_PassRenderNode_H___
#define ___C3D_PassRenderNode_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used link a pass and a shader program.
	\~french
	\brief		Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct PassRenderNodeUniforms
	{
		C3D_API PassRenderNodeUniforms( UniformBuffer & p_passUbo );

		//!\~english	The pass UBO.
		//!\~french		L'UBO de passe.
		UniformBuffer & m_passUbo;
		//!\~english	The pass diffuse colour.
		//!\~french		La couleur diffuse de passe.
		Uniform4r & m_diffuse;
		//!\~english	The pass specular colour.
		//!\~french		La couleur spéculaire de passe.
		Uniform4r & m_specular;
		//!\~english	The pass emissive colour.
		//!\~french		La couleur émissive de passe.
		Uniform4r & m_emissive;
		//!\~english	The pass shininess.
		//!\~french		L'exposante de passe.
		Uniform1f & m_shininess;
		//!\~english	The pass opacity.
		//!\~french		L'opacité de passe.
		Uniform1f & m_opacity;
		//!\~english	The pass environment map index.
		//!\~french		L'indice de la texture d'environnement de la passe.
		Uniform1f & m_environmentIndex;
		//!\~english	The pass refraction ratio.
		//!\~french		Le ratio de réfraction de la passe.
		Uniform1f & m_refractionRatio;
		//!\~english	The pass textures.
		//!\~french		Les textures de la passe.
		std::map< uint32_t, std::reference_wrapper< PushUniform1s > > m_textures;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used link a pass and a shader program.
	\~french
	\brief		Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct PassRenderNode
		: public PassRenderNodeUniforms
	{
		C3D_API PassRenderNode( Pass & p_pass
			, ShaderProgram const & p_program
			, UniformBuffer & p_passUbo );

		//!\~english	The pass.
		//!\~french		La passe.
		Pass & m_pass;
	};
}

#endif
