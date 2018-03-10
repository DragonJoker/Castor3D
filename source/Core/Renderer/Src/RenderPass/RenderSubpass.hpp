/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderSubpass_HPP___
#define ___Renderer_RenderSubpass_HPP___
#pragma once

#include "AttachmentReference.hpp"
#include "RenderSubpassState.hpp"
#include "SubpassDescription.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une sous passe de rendu.
	*\~english
	*\brief
	*	Describes a render subpass.
	*/
	class RenderSubpass
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] pipelineBindPoint
		*	Specifies whether this is a compute or graphics subpass.
		*\param[in] state
		*	The state wanted when beginning the subpass.
		*\param[in] inputAttaches
		*	Lists which of the render pass’s attachments can be read in the fragment shader stage during the subpass, and what layout each attachment will be in during the subpass.
		*	Each element of the array corresponds to an input attachment unit number in the shader, i.e. if the shader declares an input variable layout(input_attachment_index=X, set=Y, binding=Z) then it uses the attachment provided in \p inputAttaches[X].
		*	Input attachments must also be bound to the pipeline with a descriptor set, with the input attachment descriptor written in the location (set=Y, binding=Z).
		*	Fragment shaders can use subpass input variables to access the contents of an input attachment at the fragment’s (x, y, layer) framebuffer coordinates.
		*\param[in] colourAttaches
		*	Lists which of the render pass’s attachments will be used as color attachments in the subpass, and what layout each attachment will be in during the subpass.
		*	Each element of the array corresponds to a fragment shader output location, i.e. if the shader declared an output variable layout(location=X) then it uses the attachment provided in \p colourAttaches[X].
		*\param[in] resolveAttaches
		*	Lists which of the render pass’s attachments are resolved to at the end of the subpass, and what layout each attachment will be in during the multisample resolve operation.
		*	If \p resolveAttachments is not empty, each of its elements corresponds to a color attachment (the element in \p colorAttaches at the same index), and a multisample resolve operation is defined for each attachment.
		*	At the end of each subpass, multisample resolve operations read the subpass’s color attachments, and resolve the samples for each pixel to the same pixel location in the corresponding resolve attachments, unless the resolve attachment index is renderer::AttachmentUnused.
		*	If the first use of an attachment in a render pass is as a resolve attachment, then the loadOp is effectively ignored as the resolve is guaranteed to overwrite all pixels in the render area.
		*\param[in] depthAttach
		*	Specifies which attachment will be used for depth/stencil data and the layout it will be in during the subpass.
		*	Setting the attachment index to renderer::AttachmentUnused or leaving this pointer as \p nullptr indicates that no depth/stencil attachment will be used in the subpass.
		*\param[in] reserveAttaches
		*	An array of render pass attachment indices describing the attachments that are not used by a subpass, but whose contents must be preserved throughout the subpass.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] pipelineBindPoint
		*	Définit si c'est une sous-passe de calcul ou graphique.
		*\param[in] state
		*	L'état voulu au démarrage de la sous-passe.
		*\param[in] inputAttaches
		*	Liste quelles attaches de la passe de rendu peuvent être lues depuis le fragment shader durant la sous-passe, et quel layout chaque attache aura pendant la sous-passe.
		*	Chaque élément du tableau correspond au numéro de l'unité d'attache d'entrée dans le shader, i.e. si le shader déclare une variable d'entrée layout(input_attachment_index=X, set=Y, binding=Z) alors il utilise l'attache fournie dans \p inputAttaches[X].
		*	Les attaches d'entrée doivent aussi être liées au pipeline via un ensemble de descripteurs, en avec le descripteur d'attache d'entrée à la location(set=Y, binding=Z).
		*	Les fragment shaders peuvent utliser les variables d'entrée de sous-passe pour accéder au contenu d'une attache d'entrée aux coordonnées de tampon d'image du fragment (x, y, layer).
		*\param[in] colourAttaches
		*	Liste quelles attaches de la passe de rendu seront utilisées comme attaches couleur durant la sous-passe, et quel layout chaque attache aura pendant la sous-passe.
		*	Chaque élément du tableau correspond à la position de sortie d'un fragment shader, i.e. si le shader a déclaré une variable de sortie layout(location=X), alors il utilise l'attache fournie dans \p colourAttaches[X].
		*\param[in] resolveAttaches
		*	Liste quelles attaches de la passe de rendu sont résolues à la fin de la sous-pass, et quel layout chaque attache aura pendant l'opération de résolution de multi-échantillon.
		*	Si \p resolveAttachments n'est pas vide, chacun de ses éléments correspond à une attache couleur (l'élément de \p colourAttaches au même indice), et une opération de résolution de multi-échantillon est défini pour chaque attache.
		*	A la fin de chaque sous-passe, les opérations de résolution de multi-échantillon lisent les attaches couleur de la sous-passe, et résolvent les échantillons pour chaque pixel à la même position de que le pixel dans l'attache de résolution, à moins que l'indice de l'attache de résolution soit renderer::AttachmentUnused.
		*	Si la première utilisation d'une attache dans la sous-passe est une attache de résolution, alors le loadOp est ignoré, car la résolution garantit la réécriture de tous les pixels dans la zone de rendu.
		*\param[in] depthAttach
		*	Définit quelle attache sera utilisée pour les données de profondeur/stencil et le layout qu'elle aura durant la sous-passe.
		*	Si ce pointeur est \p nullptr, ou si l'indice de l'attache vaut renderer::AttachmentUnused, alors aucune attache de profondeur/stencil ne sera utilisée dans la sous-passe.
		*\param[in] reserveAttaches
		*	Un tableau d'indices d'attaches de la passe de rendu qui ne seront pas utilisées par une sous-passe, mais dont le contenu doit être préservé au travers de la sous-passe.
		*/
		RenderSubpass( PipelineBindPoint pipelineBindPoint
			, RenderSubpassState const & state
			, AttachmentReferenceArray const & inputAttaches
			, AttachmentReferenceArray const & colourAttaches
			, AttachmentReferenceArray const & resolveAttaches
			, AttachmentReference const * depthAttach
			, UInt32Array const & reserveAttaches );
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] pipelineBindPoint
		*	Specifies whether this is a compute or graphics subpass.
		*\param[in] state
		*	The state wanted when beginning the subpass.
		*\param[in] colourAttaches
		*	Lists which of the render pass’s attachments will be used as color attachments in the subpass, and what layout each attachment will be in during the subpass.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] pipelineBindPoint
		*	Définit si c'est une sous-passe de calcul ou graphique.
		*\param[in] state
		*	L'état voulu au démarrage de la sous-passe.
		*\param[in] colourAttaches
		*	Liste quelles attaches de la passe de rendu seront utilisées comme attaches couleur durant la sous-passe, et quel layout chaque attache aura pendant la sous-passe.
		*/
		RenderSubpass( PipelineBindPoint pipelineBindPoint
			, RenderSubpassState const & state
			, AttachmentReferenceArray const & colourAttaches );
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] pipelineBindPoint
		*	Specifies whether this is a compute or graphics subpass.
		*\param[in] state
		*	The state wanted when beginning the subpass.
		*\param[in] colourAttaches
		*	Lists which of the render pass’s attachments will be used as color attachments in the subpass, and what layout each attachment will be in during the subpass.
		*\param[in] depthAttach
		*	Specifies which attachment will be used for depth/stencil data and the layout it will be in during the subpass.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] pipelineBindPoint
		*	Définit si c'est une sous-passe de calcul ou graphique.
		*\param[in] state
		*	L'état voulu au démarrage de la sous-passe.
		*\param[in] colourAttaches
		*	Liste quelles attaches de la passe de rendu seront utilisées comme attaches couleur durant la sous-passe, et quel layout chaque attache aura pendant la sous-passe.
		*\param[in] depthAttach
		*	Définit quelle attache sera utilisée pour les données de profondeur/stencil et le layout qu'elle aura durant la sous-passe.
		*/
		RenderSubpass( PipelineBindPoint pipelineBindPoint
			, RenderSubpassState const & state
			, AttachmentReferenceArray const & colourAttaches
			, AttachmentReference const & depthAttach );

		inline SubpassDescription const & getDescription()const
		{
			return m_description;
		}

		inline RenderSubpassState const & getNeededState()const
		{
			return m_neededState;
		}

	private:
		SubpassDescription m_description{};
		RenderSubpassState m_neededState{};
	};
}

#endif
