/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderModule_HPP___
#define ___Renderer_ShaderModule_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Gère un module shader qu'un pipeline peut utiliser.
	*/
	class ShaderModule
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructeur.
		*\param[in] stage
		*	The module's shader stage.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		ShaderModule( ShaderStageFlag stage );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~ShaderModule() = default;
		/**
		*\~english
		*\brief
		*	Creates a shader module.
		*\param[in] shader
		*	The GLSL shader code.
		*\~french
		*\brief
		*	Crée un module de shader.
		*\param[in] shader
		*	Le code GLSL du shader.
		*/
		virtual void loadShader( std::string const & shader ) = 0;
		/**
		*\~english
		*\brief
		*	Loads a SPIR-V shader.
		*\param[in] shader
		*	The SPIR-V shader code.
		*\~french
		*\brief
		*	Charge un programme SPIR-V.
		*\param[in] shader
		*	Le code SPIR-V du shader.
		*/
		virtual void loadShader( ByteArray const & shader ) = 0;
		
		inline ShaderStageFlag getStage()const
		{
			return m_stage;
		}

	private:
		ShaderStageFlag m_stage;
	};
}

#endif
