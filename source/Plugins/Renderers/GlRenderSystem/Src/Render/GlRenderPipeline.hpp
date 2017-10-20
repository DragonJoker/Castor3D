/*
See LICENSE file in root folder
*/
#ifndef ___GL_PIPELINE_H___
#define ___GL_PIPELINE_H___

#include "Common/GlHolder.hpp"

#include <Render/RenderPipeline.hpp>

namespace GlRender
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
	class GlRenderPipeline
		: public castor3d::RenderPipeline
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL api.
		 *\param[in]	renderSystem	The parent RenderSystem.
		 *\param[in]	p_dsState		The depth stencil state.
		 *\param[in]	p_rsState		The rateriser state.
		 *\param[in]	p_bdState		The blend state.
		 *\param[in]	p_msState		The multisample state.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_flags			The creation flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_gl			L'api OpenGL.
		 *\param[in]	renderSystem	Le RenderSystem parent.
		 *\param[in]	p_dsState		L'état de stencil et profondeur.
		 *\param[in]	p_rsState		L'état de rastériseur.
		 *\param[in]	p_bdState		L'état de mélange.
		 *\param[in]	p_msState		L'état de multi-échantillonnage.
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_flags			Les indicateurs de création.
		 */
		GlRenderPipeline( OpenGl & p_gl, GlRenderSystem & renderSystem
					, castor3d::DepthStencilState && p_dsState
					, castor3d::RasteriserState && p_rsState
					, castor3d::BlendState && p_bdState
					, castor3d::MultisampleState && p_msState
					, castor3d::ShaderProgram & p_program
					, castor3d::PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlRenderPipeline();
		/**
		 *\~english
		 *\brief		Applies the pipeline.
		 *\~french
		 *\brief		Applique le pipeline.
		 */
		void apply()const override;
	};
}

#endif
