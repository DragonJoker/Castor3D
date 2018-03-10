/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SubpassContents_HPP___
#define ___Renderer_SubpassContents_HPP___
#pragma once

namespace renderer
{
	/**
	*\brief
	*	Décrit comment les commandes d'une sous-passe sont fournies.
	*/
	enum class SubpassContents
		: uint32_t
	{
		//! Aucun tampon de commandes secondaire.
		eInline,
		//! Uniquement des tampons de commandes secondaire.
		eSecondaryCommandBuffers,
	};
}

#endif
