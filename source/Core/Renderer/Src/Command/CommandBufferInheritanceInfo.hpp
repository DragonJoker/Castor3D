/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandBufferInheritanceInfo_HPP___
#define ___Renderer_CommandBufferInheritanceInfo_HPP___
#pragma once

#include "Image/ImageSubresourceLayers.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies command buffer inheritance info.
	*\~french
	*\brief
	*	Définit Les informations d'héritage de tampon de commandes.
	*/
	struct CommandBufferInheritanceInfo
	{
		/**
		*\~english
		*\brief
		*	Defines which render pass the command buffer will be compatible with and can be executed within.
		*\remarks
		*	If the command buffer will not be executed within a render pass instance, this member is ignored.
		*\~french
		*\brief
		*	Définit avec quelle passe de rendu le tampon de commandes va être compatible et où il va s'exécuter.
		*\remarks
		*	Si le tampon de commandes ne sera pas exécuté dans une instance de passe de rendu, ce membre est ignoré
		*/
		RenderPass const * renderPass;
		/**
		*\~english
		*\brief
		*	The index of the subpass within the render pass instance that the command buffer will be executed within.
		*\remarks
		*	If the command buffer will not be executed within a render pass instance, this member is ignored.
		*\~french
		*\brief
		*	L'indice de la sous-passe au sein de l'instance de passe de rendu dans laquelle le tampon de commandes va être exécuté.
		*\remarks
		*	Si le tampon de commandes ne sera pas exécuté dans une instance de passe de rendu, ce membre est ignoré
		*/
		uint32_t subpass;
		/**
		*\~english
		*\brief
		*	Optionally refers to the framebuffer that the command buffer will be rendering to, if executed within a render pass instance.
		*	It can be \p nullptr if the framebuffer is not known, or if the command buffer will not be executed within a render pass instance.
		*\remarks
		*	Specifying the exact framebuffer that the secondary command buffer will be executed may result in better performance at command buffer execution time.
		*\~french
		*\brief
		*	Réfère optionnellement au tampon d'images dans lequel le tampon de commandes va dessiner, si exécuté depuis une passe de rendu.
		*	Il peut être \p nullptr si inconnu, ou si le tampon de commandes ne sera pas exécuté depuis une instance de passe de rendu.
		*\remarks
		*	Spécifier le tampon d'images dans lequel le tampon de commandes secondaire sera exécuté peut engendrer de meilleures performances à l'exécution du tampon de commandes.
		*/
		FrameBuffer const * framebuffer;
		/**
		*\~english
		*\brief
		*	Indicates whether the command buffer can be executed while an occlusion query is active in the primary command buffer.
		*	If this is \p true, then this command buffer can be executed whether the primary command buffer has an occlusion query active or not.
		*	If this is \p false, then the primary command buffer must not have an occlusion query active..
		*\~french
		*\brief
		*	Indique si le tampon de commandes peut être exécuté alors qu'une requête d'occlusion est active sur le tampon de commandes primaire.
		*	Si \p true, alors ce tampon de commandes peut être exécuté qu'une requête d'occlusion soit active sur le tampon de commandes primaire ou non.
		*	Si \p false, alors le tampon de commandes primaire ne doit pas avoir de requête d'occlusion active.
		*/
		bool occlusionQueryEnable;
		/**
		*\~english
		*\brief
		*	The query flags that can be used by an active occlusion query in the primary command buffer when this secondary command buffer is executed.
		*	If this value includes the renderer::QueryControlFlag::ePrecise bit, then the active query can return boolean results or actual sample counts.
		*	If this bit is not set, then the active query must not use the renderer::QueryControlFlag::ePrecise bit.
		*\~french
		*\brief
		*	Les indicateurs de requête qui peuvent être utilisés par une requête d'occlusion active sur le tampon de commandes primaire lorsque celui-ci est exécuté.
		*	Si cette valeur contient renderer::QueryControlFlag::ePrecise, alors la requête active peut renvoyer des résultats booléens ou le compte des échantillons.
		*	Sinon la requête active ne doit pas utiliser renderer::QueryControlFlag::ePrecise.
		*/
		QueryControlFlags queryFlags;
		/**
		*\~english
		*\brief
		*	A bitmask of renderer::QueryPipelineStatisticFlag specifying the set of pipeline statistics that can be counted by an active query in the primary command buffer when this secondary command buffer is executed.
		*	If this value includes a given bit, then this command buffer can be executed whether the primary command buffer has a pipeline statistics query active that includes this bit or not.
		*	If this value excludes a given bit, then the active pipeline statistics query must not be from a query pool that counts that statistic.
		*\~french
		*\brief
		*	Un masque binaire de renderer::QueryPipelineStatisticFlag définissant l'ensemble de statistiques de pipeline pouvant être comptées dans le tampon de commandes primaire lorsque celui-ci est exécuté.
		*	Si cette valeur inclut un bit donné, alors ce tampon de commande peut être exécuté que le tampon de commandes primaire ait une requête de statistiquess de pipeline active incluant ce bit ou non.
		*	Si cette valeur exclut un bit donné, alors la requête active de statistiques de pipeline ne doit pas provenir d'un tampon de requêtes comptant cette statistique.
		*/
		QueryPipelineStatisticFlags pipelineStatistics;
	};
}

#endif
