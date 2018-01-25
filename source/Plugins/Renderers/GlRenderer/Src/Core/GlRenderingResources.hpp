/**
*\file
*	RenderingResources.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_RenderingResources_HPP___
#define ___VkRenderer_RenderingResources_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Core/RenderingResources.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class RenderingResources
		: public renderer::RenderingResources
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le Device parent.
		*/
		RenderingResources( renderer::Device const & device );
		/**
		*\brief
		*	Attend que le tampon de commandes soit prêt à l'enregistrement.
		*\param[in] timeout
		*	Le temps à attendre pour le signalement.
		*\return
		*	\p true si l'attente n'est pas sortie en timeout.
		*/
		bool waitRecord( uint32_t timeout )override;
	};
}

#endif
