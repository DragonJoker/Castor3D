/*
See LICENSE file in root folder
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
