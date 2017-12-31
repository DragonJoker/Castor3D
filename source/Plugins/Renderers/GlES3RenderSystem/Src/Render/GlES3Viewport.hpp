/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_VIEWPORT_H___
#define ___C3DGLES3_VIEWPORT_H___

#include "Common/GlES3Holder.hpp"

#include <Render/Viewport.hpp>

namespace GlES3Render
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
	class GlES3Viewport
		: public Castor3D::IViewportImpl
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_viewport		The parent viewport.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_viewport		Le viewport parent.
		 */
		GlES3Viewport( GlES3RenderSystem & p_renderSystem, Castor3D::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3Viewport();
		/**
		 *\copydoc		Castor3D::IViewportImpl::Apply
		 */
		void Apply()const override;
	};
}

#endif
