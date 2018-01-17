/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPassState_HPP___
#define ___Renderer_RenderPassState_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Description d'un état d'une passe de rendu.
	*/
	class RenderPassState
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] pipelineStage
		*	Les indicateurs d'état du pipeline.
		*\param[in] access
		*	Les indicateurs d'accès.
		*\param[in] imageLayouts
		*	Le layout des attaches.
		*/
		RenderPassState( PipelineStageFlags pipelineStage
			, AccessFlags access
			, ImageLayoutArray const & imageLayouts );
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
		/**
		*\return
		*	Le début du tableau de layouts d'images.
		*/
		inline ImageLayoutArray::const_iterator begin()const
		{
			return m_imageLayouts.begin();
		}
		/**
		*\return
		*	La fin du tableau de layouts d'images.
		*/
		inline ImageLayoutArray::const_iterator end()const
		{
			return m_imageLayouts.end();
		}

	private:
		PipelineStageFlags m_pipelineStage;
		AccessFlags m_access;
		ImageLayoutArray m_imageLayouts;
	};
}

#endif
