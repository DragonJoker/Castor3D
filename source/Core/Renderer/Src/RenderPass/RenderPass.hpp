/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPass_HPP___
#define ___Renderer_RenderPass_HPP___
#pragma once

#include "RenderPass/AttachmentDescription.hpp"
#include "RenderPass/SubpassDescription.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une passe de rendu (pouvant contenir plusieurs sous-passes).
	*\~english
	*\brief
	*	Describes a render pass (which can contain one or more render subpasses).
	*/
	class RenderPass
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] createInfo
		*	The creation informations.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] createInfo
		*	Les informations de création.
		*/
		RenderPass( Device const & device
			, RenderPassCreateInfo const & createInfo );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~RenderPass() = default;
		/**
		*\~french
		*\brief
		*	Crée un tampon d'images compatible avec la passe de rendu.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*\param[in] attaches
		*	Les attaches pour le tampon d'images à créer.
		*\return
		*	Le FrameBuffer créé.
		*\~english
		*\brief
		*	Creates a frame buffer compatible with this render pass.
		*\remarks
		*	If the compatibility between wanted views and the render pass' formats
		*	is not possible, a std::runtime_error will be thrown.
		*\param[in] dimensions
		*	The frame buffer's dimensions.
		*\param[in] attaches
		*	The attaches for the frame buffer to create.
		*\return
		*	The created frame buffer.
		*/
		virtual FrameBufferPtr createFrameBuffer( Extent2D const & dimensions
			, FrameBufferAttachmentArray && attaches )const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline size_t getAttachmentCount()const
		{
			return m_attachments.size();
		}

		inline AttachmentDescriptionArray const & getAttachments()const
		{
			return m_attachments;
		}

		inline size_t getSubpassCount()const
		{
			return m_subpasses.size();
		}

		inline SubpassDescriptionArray const & getSubpasses()const
		{
			return m_subpasses;
		}
		/**@}*/

	private:
		SubpassDescriptionArray m_subpasses;
		AttachmentDescriptionArray m_attachments;
	};
}

#endif
