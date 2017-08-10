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
#ifndef ___C3D_ComputePipeline_H___
#define ___C3D_ComputePipeline_H___

#include "Castor3DPrerequisites.hpp"

#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"

#include <Math/SquareMatrix.hpp>
#include <Design/OwnedBy.hpp>

#include <stack>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		24/11/2016
	\~english
	\brief		The computing pipeline.
	\~french
	\brief		Le pipeline de calcul.
	*/
	class ComputePipeline
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The parent RenderSystem.
		 *\param[in]	p_program		The shader program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem parent.
		 *\param[in]	p_program		Le programme shader.
		 */
		C3D_API explicit ComputePipeline( RenderSystem & renderSystem, ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ComputePipeline();
		/**
		 *\~english
		 *\brief		Cleans up the pipeline.
		 *\~french
		 *\brief		Nettoie le pipeline.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Runs the computation.
		 *\param[in]	p_workgroups	The worgroups count.
		 *\param[in]	p_workgroupSize	The worgroup size.
		 *\param[in]	p_barriers		The memory barriers.
		 *\~french
		 *\brief		Lance les calculs.
		 *\param[in]	p_workgroups	Le nombre de groupes de travail.
		 *\param[in]	p_workgroupSize	La taille d'un groupe de travail.
		 *\param[in]	p_barriers		Les barri�res m�moire.
		 */
		C3D_API virtual void run( castor::Point3ui const & p_workgroups
			, castor::Point3ui const & p_workgroupSize
			, MemoryBarriers const & p_barriers )const = 0;
		/**
		 *\~english
		 *\return		The shader program.
		 *\~french
		 *\return		Le programme shader.
		 */
		inline ShaderProgram & getProgram()
		{
			return m_program;
		}

	protected:
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		ShaderProgram & m_program;
	};
}

#endif
