﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SHADER_PLUGIN_H___
#define ___C3D_SHADER_PLUGIN_H___

#include "Plugin.hpp"
#include "PluginException.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Shader program Plugin class
	\~french
	\brief		Classe de plugin de programmes shader
	*/
	class C3D_API ShaderPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef ShaderProgramBaseSPtr	CreateShaderFunction( RenderSystem * p_pRenderSystem );
		typedef IPipelineImpl 	*		CreatePipelineFunction( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem );
		typedef void					DestroyPipelineFunction( IPipelineImpl * p_pPipeline );
		typedef eSHADER_LANGUAGE		GetShaderLanguageFunction();

		typedef CreateShaderFunction		*	PCreateShaderFunction;
		typedef CreatePipelineFunction		*	PCreatePipelineFunction;
		typedef DestroyPipelineFunction		*	PDestroyPipelineFunction;
		typedef GetShaderLanguageFunction	*	PGetShaderLanguageFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		ShaderPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ShaderPlugin();
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram object
		 *\param[in]	p_pRenderSystem	The render system
		 *\return		The created ShaderProgram instance
		 *\~french
		 *\brief		Crée un objet ShaderProgram
		 *\param[in]	p_pRenderSystem	Le render system
		 *\return		L'instance de ShaderProgram créée
		 */
		ShaderProgramBaseSPtr CreateShader( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Creates a IPipelineImpl object
		 *\param[in]	p_pPipeline		The parent pipeline
		 *\param[in]	p_pRenderSystem	The render system
		 *\return		The created IPipelineImpl instance
		 *\~french
		 *\brief		Crée un objet IPipelineImpl
		 *\param[in]	p_pPipeline		Le pipeline parent
		 *\param[in]	p_pRenderSystem	Le render system
		 *\return		L'instance de IPipelineImpl créée
		 */
		IPipelineImpl * CreatePipeline( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destroys the given IPipelineImpl
		 *\param[in]	p_pPipeline	The IPipelineImpl
		 *\~french
		 *\brief		Détruit le IPipelineImpl donné
		 *\param[in]	p_pPipeline	Le IPipelineImpl
		 */
		void DestroyPipeline( IPipelineImpl * p_pPipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader language
		 *\return		The extension
		 *\~french
		 *\brief		Récupère le langage de shader
		 *\return		L'estenxion
		 */
		eSHADER_LANGUAGE GetShaderLanguage();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		ShaderPlugin( ShaderPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		ShaderPlugin( ShaderPlugin && p_plugin );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		ShaderPlugin & operator =( ShaderPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		ShaderPlugin & operator =( ShaderPlugin && p_plugin );

	private:
		PCreateShaderFunction m_pfnCreateShader;
		PCreatePipelineFunction m_pfnCreatePipeline;
		PDestroyPipelineFunction m_pfnDestroyPipeline;
		PGetShaderLanguageFunction m_pfnGetShaderLanguage;
	};
}

#endif

