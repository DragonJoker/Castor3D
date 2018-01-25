/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Shader/ShaderProgram.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Gère les différents ShaderModules qu'un programme shader peut utiliser.
	*/
	class ShaderProgram
		: public renderer::ShaderProgram
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		ShaderProgram( renderer::Device const & device );
		/**
		*\brief
		*	Destructeur.
		*/
		~ShaderProgram();
		/**
		*\brief
		*	Crée un module de shader et l'ajoute à la liste.
		*\param[in] shader
		*	Le code GLSL du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		void createModule( std::string const & shader
			, renderer::ShaderStageFlag stage )override;
		/**
		*\brief
		*	Crée un module de shader et l'ajoute à la liste.
		*\param[in] shader
		*	Le code SPIR-V du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		void createModule( renderer::ByteArray const & shader
			, renderer::ShaderStageFlag stage )override;
		/**
		*\brief
		*	Lie les modules du programme.
		*/
		void link()override;
		/**
		*\brief
		*	Dit si le SPIR-V est supporté.
		*/
		inline bool isSPIRVSupported()override
		{
			return false;
		}
		/**
		*\brief
		*	Dit si le GLSL est supporté.
		*/
		inline bool isGLSLSupported()override
		{
			return true;
		}
		/**
		*\brief
		*	Le début du tableau de modules.
		*/
		inline GLuint getProgram()const
		{
			return m_program;
		}

	private:
		GLuint m_program;
		renderer::UInt32Array m_shaders;
	};
}
