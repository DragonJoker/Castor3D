/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ShaderProgram_HPP___
#define ___Renderer_ShaderProgram_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Helper class used to create ShaderModule.
	*\~french
	*\brief
	*	Classe d'aide à la création de ShaderModule.
	*/
	class ShaderProgram
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		ShaderProgram( Device const & device );
		/**
		*\~english
		*\brief
		*	Creates a shader module.
		*\param[in] shader
		*	The GLSL shader code.
		*\param[in] stage
		*	The module's shader stage.
		*\~french
		*\brief
		*	Crée un module de shader.
		*\param[in] shader
		*	Le code GLSL du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		virtual ShaderModulePtr createModule( std::string const & shader
			, ShaderStageFlag stage );
		/**
		*\~english
		*\brief
		*	Creates a shader module.
		*\param[in] shader
		*	The SPIR-V shader code.
		*\param[in] stage
		*	The module's shader stage.
		*\~french
		*\brief
		*	Crée un module de shader.
		*\param[in] shader
		*	Le code SPIR-V du shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		virtual ShaderModulePtr createModule( ByteArray const & shader
			, ShaderStageFlag stage );

	private:
		Device const & m_device;
	};
}

#endif
