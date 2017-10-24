/* See LICENSE file in root folder */
#ifndef ___TRS_VIEWPORT_H___
#define ___TRS_VIEWPORT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Render/Viewport.hpp>

namespace TestRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/04/2016
	\~english
	\brief		Render API specific viewport implementation.
	\~french
	\brief		Implàmentation de viewport spàcifique à l'API de rendu.
	*/
	class TestViewport
		: public castor3d::IViewportImpl
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
		TestViewport( TestRenderSystem & p_renderSystem, castor3d::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TestViewport();
		/**
		 *\copydoc		castor3d::IViewportImpl::Apply
		 */
		void apply()const override;
	};
}

#endif
