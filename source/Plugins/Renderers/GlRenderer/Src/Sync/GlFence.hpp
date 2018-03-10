/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Sync/Fence.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe permettant la synchronisation des opérations sur une file.
	*/
	class Fence
		: public renderer::Fence
	{
	public:
		/**
		*\brief
		*	Constructeur
		*\param[in] device
		*	Le device parent.
		*\param[in] flags
		*	Les indicateurs de création de la barrière.
		*/ 
		Fence( renderer::Device const & device
			, renderer::FenceCreateFlags flags = 0 );
		/**
		*\brief
		*	Destructeur
		*/
		~Fence();
		/**
		*\brief
		*	Attend que la barrière soit signalée.
		*\param[in] timeout
		*	Le temps à attendre pour le signalement.
		*\return
		*	\p WaitResult::eSuccess ou \p WaitResult::eTimeOut en cas de succès.
		*/ 
		renderer::WaitResult wait( uint32_t timeout )const override;
		/**
		*\brief
		*	Remet la barrière en non signalée.
		*/ 
		void reset()const override;

	private:
		mutable GLsync m_fence{ nullptr };
	};
}
