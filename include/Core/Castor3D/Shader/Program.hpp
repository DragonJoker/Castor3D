/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderProgram_H___
#define ___C3D_ShaderProgram_H___

#include "ShaderModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>

#include <ShaderAST/Shader.hpp>
#include <ShaderAST/Visitors/SelectEntryPoint.hpp>

#include <ashespp/Core/Device.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	class ShaderProgram
		: public castor::Named
		, public castor::OwnedBy< RenderSystem >
		, public std::enable_shared_from_this< ShaderProgram >
	{
		friend class castor::TextWriter< castor3d::ShaderProgram >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name			The program name.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom du programme.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		C3D_API explicit ShaderProgram( castor::String const & name
			, RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderProgram();
		/**
		 *\~english
		 *\brief		Sets the shader file.
		 *\param[in]	target		The shader module concerned.
		 *\param[in]	pathFile	The file name.
		 *\~french
		 *\brief		Définit le fichier du shader.
		 *\param[in]	target		Le module shader concerné.
		 *\param[in]	pathFile	Le nom du fichier.
		 */
		C3D_API void setFile( VkShaderStageFlagBits target, castor::Path const & pathFile );
		/**
		 *\~english
		 *\brief		Sets the shader source.
		 *\param[in]	target	The shader object concerned.
		 *\param[in]	source	The source code.
		 *\~french
		 *\brief		Définit la source du shader.
		 *\param[in]	target	Le shader object concerné.
		 *\param[in]	source	Le code de la source.
		 */
		C3D_API void setSource( VkShaderStageFlagBits target, castor::String const & source );
		/**
		 *\~english
		 *\brief		Sets the shader source.
		 *\param[in]	target	The shader object concerned.
		 *\param[in]	shader	The source shader.
		 *\~french
		 *\brief		Définit la source du shader.
		 *\param[in]	target	Le shader object concerné.
		 *\param[in]	shader	Le shader de la source.
		 */
		C3D_API void setSource( VkShaderStageFlagBits target, ShaderPtr shader );
		/**
		 *\~english
		 *\brief		Sets the program source.
		 *\param[in]	shader	The source shader.
		 *\~french
		 *\brief		Définit la source du programme.
		 *\param[in]	shader	Le shader de la source.
		 */
		C3D_API void setSource( ShaderPtr shader );
		/**
		 *\~english
		 *\param[in]	stage	The shader stage.
		 *\return		\p true if the shader program has a source for given shader stage.
		 *\~french
		 *\param[in]	stage	Le shader stage.
		 *\return		\p true si le shader a une source pour le shader stage donné.
		 */
		C3D_API bool hasSource( ast::ShaderStage stage )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		ProgramModule const & getModule()const noexcept
		{
			return m_module;
		}

		ashes::PipelineShaderStageCreateInfoArray const & getStates()const noexcept
		{
			return m_states;
		}
		/**@}*/

	protected:
		std::map< VkShaderStageFlagBits, castor::Path > m_files;
		ProgramModule m_module;
		ashes::PipelineShaderStageCreateInfoArray m_states;
	};

	C3D_API SpirVShader const & compileShader( RenderDevice const & device
		, ShaderModule & module );
	C3D_API SpirVShader const & compileShader( RenderSystem & renderSystem
		, ShaderModule & module );
	C3D_API SpirVShader const & compileShader( RenderDevice const & device
		, ProgramModule & module
		, ast::EntryPointConfig const & entryPoint );
	C3D_API SpirVShader const & compileShader( RenderSystem & renderSystem
		, ProgramModule & module
		, ast::EntryPointConfig const & entryPoint );
	C3D_API ashes::PipelineShaderStageCreateInfoArray makeProgramStates( RenderDevice const & device
		, ProgramModule & programModule
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt );
	C3D_API ast::ShaderStage getShaderStage( VkShaderStageFlagBits value );
	C3D_API VkShaderStageFlagBits getVkShaderStage( ast::ShaderStage value );
	C3D_API VkShaderStageFlagBits getVkShaderStage( ast::EntryPoint value );
	C3D_API ast::EntryPoint getEntryPointType( VkShaderStageFlagBits value );

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( ashes::Device const & device
		, VkShaderStageFlagBits stage
		, SpirVShader const & code
		, std::string const & name
		, std::string mainFuncName = "main"
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt )
	{
		auto module = device.createShaderModule( name + "ShdMod" + "_" + ashes::getName( stage )
			, code.spirv );
		return ashes::PipelineShaderStageCreateInfo
		{
			0u,
			stage,
			std::move( module ),
			std::move( mainFuncName ),
			std::move( specialization ),
		};
	}

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( RenderDevice const & device
		, ShaderModule & shaderModule
		, std::string mainFuncName = "main"
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt )
	{
		return makeShaderState( *device
			, shaderModule.stage
			, compileShader( device, shaderModule )
			, shaderModule.name
			, std::move( mainFuncName )
			, std::move( specialization ) );
	}
}

#endif
