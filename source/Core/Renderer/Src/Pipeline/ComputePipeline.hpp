/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ComputePipeline_HPP___
#define ___Renderer_ComputePipeline_HPP___
#pragma once

#include "Miscellaneous/ComputePipelineCreateInfo.hpp"

namespace renderer
{
	/**
	*\brief
	*	Un pipeline de rendu.
	*/
	class ComputePipeline
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructir.
		*\param[in] device
		*	The parent logical device.
		*\param[in] layout
		*	The pipeline layout.
		*\param[in] createInfo
		*	The pipeline creation informations.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le device parent.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] program
		*	Les informations de creation du pipeline.
		*/
		ComputePipeline( Device const & device
			, PipelineLayout const & layout
			, ComputePipelineCreateInfo && createInfo );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~ComputePipeline() = default;
		/**
		*\~english
		*\return
		*	The parent layout.
		*\~french
		*\return
		*	Le layout parent.
		*/
		inline PipelineLayout const & getLayout()const
		{
			return m_layout;
		}

	protected:
		ComputePipelineCreateInfo m_createInfo;
		PipelineLayout const & m_layout;
	};
}

#endif
