/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderSubpassState_HPP___
#define ___Renderer_RenderSubpassState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Description d'un état d'une passe de rendu.
	*/
	class RenderSubpassState
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] pipelineStage
		*	Les indicateurs d'état du pipeline.
		*\param[in] access
		*	Les indicateurs d'accès.
		*/
		RenderSubpassState( PipelineStageFlags pipelineStage
			, AccessFlags access );
		/**
		*\return
		*	Les indicateurs d'état du pipeline.
		*/
		inline PipelineStageFlags const & getPipelineStage()const
		{
			return m_pipelineStage;
		}
		/**
		*\return
		*	Les indicateurs d'accès.
		*/
		inline AccessFlags const & getAccess()const
		{
			return m_access;
		}

	private:
		PipelineStageFlags m_pipelineStage;
		AccessFlags m_access;
	};
}

#endif
