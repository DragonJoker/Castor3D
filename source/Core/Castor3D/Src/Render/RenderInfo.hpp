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
#ifndef ___C3D_RenderInfo_H___
#define ___C3D_RenderInfo_H___

#include "Castor3DPrerequisites.hpp"
#include <cstdint>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		24/07/2017
	\~english
	\brief		Holds various render times.
	\~french
	\brief		Contient les divers temps de rendu.
	*/
	struct RenderTimes
	{
		//!\~english	The render pass name.
		//!\~french		Le nom de la passe de rendu.
		Castor::String m_name;
		//!\~english	The render pass GPU time.
		//!\~french		Le temps GPU de la passe de rendu.
		Castor::Nanoseconds m_gpu;
		//!\~english	The render pass CPU time.
		//!\~french		Le temps CPU de la passe de rendu.
		Castor::Nanoseconds m_cpu;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		22/06/2017
	\~english
	\brief		Holds render informations.
	\~french
	\brief		Contient les informations de rendu.
	*/
	struct RenderInfo
	{
		//!\~english	The total vertex count.
		//!\~french		Le nombre total de sommets.
		uint32_t m_totalVertexCount{ 0u };
		//!\~english	The total face count.
		//!\~french		Le nombre total de faces.
		uint32_t m_totalFaceCount{ 0u };
		//!\~english	The total object count.
		//!\~french		Le nombre total d'objets.
		uint32_t m_totalObjectsCount{ 0u };
		//!\~english	The visible objects count.
		//!\~french		Le nombre d'objets visibles.
		uint32_t m_visibleObjectsCount{ 0u };
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t m_particlesCount{ 0u };
		//!\~english	The draw calls count.
		//!\~french		Le nombre d'appels aux fonctions de dessin.
		uint32_t m_drawCalls{ 0u };
		//!\~english	The render times.
		//!\~french		Les temps de rendu.
		std::vector< RenderTimes > m_times;
	};
}

#endif
