/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPass_HPP___
#define ___Renderer_RenderPass_HPP___
#pragma once

#include "RenderPass/RenderPassAttachment.hpp"

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
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] attaches
		*	Les attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] attaches
		*	The attachments pixels formats.
		*\param[in] subpasses
		*	The rendersubpasses (at least one is necessary).
		*\param[in] initialState
		*	The state wanted at the beginning of the pass.
		*\param[in] finalState
		*	The state attained at the end of the pass.
		*\param[in] samplesCount
		*	The samples count (for multisampling).
		*/
		RenderPass( Device const & device
			, RenderPassAttachmentArray const & attaches
			, RenderSubpassPtrArray const & subpasses
			, RenderPassState const & initialState
			, RenderPassState const & finalState
			, SampleCountFlag samplesCount = SampleCountFlag::e1 );

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
		virtual FrameBufferPtr createFrameBuffer( UIVec2 const & dimensions
			, FrameBufferAttachmentArray && attaches )const = 0;
		/**
		*\~english
		*\return
		*	The beginning of the attaches array.
		*\~french
		*\return
		*	Le début du tableau des attaches.
		*/
		inline auto begin()const
		{
			return m_attaches.begin();
		}
		/**
		*\~english
		*\return
		*	The end of the attaches array.
		*\~french
		*\return
		*	La fin du tableau des attaches.
		*/
		inline auto end()const
		{
			return m_attaches.end();
		}
		/**
		*\~english
		*\return
		*	The number of attaches.
		*\~french
		*\return
		*	Le nombre d'attaches.
		*/
		inline size_t getSize()const
		{
			return m_attaches.size();
		}

	private:
		renderer::RenderPassAttachmentArray m_attaches;
	};
}

#endif
