/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PipelineLayout_HPP___
#define ___Renderer_PipelineLayout_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Layout de pipeline.
	*/
	class PipelineLayout
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] layout
		*	Le layout des descripteurs du pipeline.
		*/
		PipelineLayout( Device const & device
			, DescriptorSetLayout const * layout );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~PipelineLayout() = default;
	};
}

#endif
