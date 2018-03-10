/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Pipeline_HPP___
#define ___Renderer_Pipeline_HPP___
#pragma once

#include "Miscellaneous/GraphicsPipelineCreateInfo.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A rendering pipeline.
	*\~french
	*\brief
	*	Un pipeline de rendu.
	*/
	class Pipeline
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The parent device.
		*\param[in] layout
		*	The pipeline layout.
		*\param[in] createInfo
		*	The creation informations.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] createInfo
		*	Les informations de création.
		*/
		Pipeline( Device const & device
			, PipelineLayout const & layout
			, GraphicsPipelineCreateInfo && createInfo );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Pipeline() = default;
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
		GraphicsPipelineCreateInfo m_createInfo;
		PipelineLayout const & m_layout;
	};
}

#endif
