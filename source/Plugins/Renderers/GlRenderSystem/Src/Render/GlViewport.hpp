/*
See LICENSE file in root folder
*/
#ifndef ___GL_VIEWPORT_H___
#define ___GL_VIEWPORT_H___

#include "Common/GlHolder.hpp"

#include <Render/Viewport.hpp>

namespace GlRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/04/2016
	\~english
	\brief		Render API specific viewport implementation.
	\~french
	\brief		Implémentation de viewport spàcifique à l'API de rendu.
	*/
	class GlViewport
		: public castor3d::IViewportImpl
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	p_viewport		The parent viewport.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	p_viewport		Le viewport parent.
		 */
		GlViewport( GlRenderSystem & renderSystem, castor3d::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlViewport();
		/**
		 *\copydoc		castor3d::IViewportImpl::Apply
		 */
		void apply()const override;
	};
}

#endif
