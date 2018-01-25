/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <RenderPass/RenderPass.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Description d'une passe de rendu (pouvant contenir plusieurs sous-passes).
	*/
	class RenderPass
		: public renderer::RenderPass
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] clear
		*	Dit si l'on veut vider le contenu des images au chargement de la passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*/
		RenderPass( renderer::Device const & device
			, std::vector< renderer::PixelFormat > const & formats
			, renderer::RenderSubpassPtrArray const & subpasses
			, renderer::RenderPassState const & initialState
			, renderer::RenderPassState const & finalState
			, bool clear
			, renderer::SampleCountFlag samplesCount );
		/**
		*\brief
		*	Crée un FrameBuffer compatible avec la passe de rendu.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*\param[in] textures
		*	Les textures voulues pour le tampon d'images à créer.
		*\return
		*	Le FrameBuffer créé.
		*/
		renderer::FrameBufferPtr createFrameBuffer( renderer::UIVec2 const & dimensions
			, renderer::TextureViewCRefArray const & textures )const override;
		/**
		*\return
		*	Le statut de vidage.
		*/
		inline bool getClear()const
		{
			return m_clear;
		}

	private:
		bool m_clear;
	};
}
