/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_ComputePipeline_H___
#define ___C3DGLES3_ComputePipeline_H___

#include "Common/GlES3Holder.hpp"

#include <Miscellaneous/ComputePipeline.hpp>

namespace GlES3Render
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		24/11/2016
	\~english
	\brief		Implementation of the computing pipeline.
	\~french
	\brief		Implémentation du pipeline de calcul.
	*/
	class GlES3ComputePipeline
		: public Castor3D::ComputePipeline
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL api.
		 *\param[in]	p_renderSystem	The parent RenderSystem.
		 *\param[in]	p_dsState		The depth stencil state.
		 *\param[in]	p_rsState		The rateriser state.
		 *\param[in]	p_bdState		The blend state.
		 *\param[in]	p_msState		The multisample state.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_flags			The creation flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_gl			L'api OpenGL.
		 *\param[in]	p_renderSystem	Le RenderSystem parent.
		 *\param[in]	p_dsState		L'état de stencil et profondeur.
		 *\param[in]	p_rsState		L'état de rastériseur.
		 *\param[in]	p_bdState		L'état de mélange.
		 *\param[in]	p_msState		L'état de multi-échantillonnage.
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_flags			Les indicateurs de création.
		 */
		GlES3ComputePipeline( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem, Castor3D::ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3ComputePipeline();
		/**
		 *\copydoc		Castor3D::ComputePipeline::Run
		 */
		void Run(
			Castor::Point3ui const & p_workgroups,
			Castor::Point3ui const & p_workgroupSize,
			Castor::FlagCombination< Castor3D::MemoryBarrier > const & p_barriers )const override;
	};
}

#endif
