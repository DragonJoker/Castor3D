/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPass_HPP___
#define ___Renderer_RenderPass_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Description d'une passe de rendu (pouvant contenir plusieurs sous-passes).
	*/
	class RenderPass
	{
	protected:
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
		RenderPass( Device const & device
			, std::vector< PixelFormat > const & formats
			, RenderSubpassPtrArray const & subpasses
			, RenderPassState const & initialState
			, RenderPassState const & finalState
			, bool clear = true
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
		virtual FrameBufferPtr createFrameBuffer( UIVec2 const & dimensions
			, TextureViewCRefArray const & textures )const = 0;
	};
}

#endif
