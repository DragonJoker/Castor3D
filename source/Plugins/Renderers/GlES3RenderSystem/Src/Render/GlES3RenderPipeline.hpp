/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_PIPELINE_H___
#define ___C3DGLES3_PIPELINE_H___

#include "Common/GlES3Holder.hpp"

#include <Render/RenderPipeline.hpp>

namespace GlES3Render
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Implementation of the rendering pipeline.
	\~french
	\brief		Implémentation du pipeline de rendu.
	*/
	class GlES3RenderPipeline
		: public Castor3D::RenderPipeline
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
		GlES3RenderPipeline( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem
					, Castor3D::DepthStencilState && p_dsState
					, Castor3D::RasteriserState && p_rsState
					, Castor3D::BlendState && p_bdState
					, Castor3D::MultisampleState && p_msState
					, Castor3D::ShaderProgram & p_program
					, Castor3D::PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3RenderPipeline();
		/**
		 *\~english
		 *\brief		Applies the pipeline.
		 *\~french
		 *\brief		Applique le pipeline.
		 */
		void Apply()const override;
	};
}

#endif
