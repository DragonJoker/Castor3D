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
#ifndef ___C3D_SceneRenderNode_H___
#define ___C3D_SceneRenderNode_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages.
	*/
	struct SceneRenderNode
	{
		C3D_API SceneRenderNode( UniformBuffer & p_sceneUbo );

		//!\~english	The scene UBO.
		//!\~french		L'UBO de scène.
		UniformBuffer & m_sceneUbo;
		//!\~english	The ambient light.
		//!\~french		La luminosité ambiante.
		Uniform4f & m_ambientLight;
		//!\~english	The background colour.
		//!\~french		La couleur de fond.
		Uniform4f & m_backgroundColour;
		//!\~english	The lights counts.
		//!\~french		Les comptes des lumières.
		Uniform4i & m_lightsCount;
		//!\~english	The camera position.
		//!\~french		La position de la caméra.
		Uniform3f & m_cameraPos;
		//!\~english	The fog type.
		//!\~french		Le type de brouillard.
		Uniform1i & m_fogType;
		//!\~english	The fog density.
		//!\~french		La densité du brouillard.
		Uniform1f & m_fogDensity;
	};
}

#endif
