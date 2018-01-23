/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderProgram_HPP___
#define ___Renderer_ShaderProgram_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Gère les différents ShaderModules qu'un programme shader peut utiliser.
	*/
	class ShaderProgram
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		ShaderProgram( Device const & device );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~ShaderProgram() = default;
		/**
		*\brief
		*	Crée un module de shader et l'ajoute à la liste.
		*\param[in] shader
		*	Le code GLSL du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		virtual void createModule( std::string const & shader
			, ShaderStageFlag stage ) = 0;
		/**
		*\brief
		*	Crée un module de shader et l'ajoute à la liste.
		*\param[in] shader
		*	Le code SPIR-V du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		virtual void createModule( ByteArray const & shader
			, ShaderStageFlag stage ) = 0;
		/**
		*\brief
		*	Lie les modules du programme.
		*/
		virtual void link() = 0;
		/**
		*\brief
		*	Dit si le SPIR-V est supporté.
		*/
		virtual bool isSPIRVSupported() = 0;
		/**
		*\brief
		*	Dit si le GLSL est supporté.
		*/
		virtual bool isGLSLSupported() = 0;
	};
}

#endif
