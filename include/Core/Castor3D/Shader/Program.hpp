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
		C3D_API void setSource( VkShaderStageFlagBits target, castor::MbString const & source );
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
		castor::Map< VkShaderStageFlagBits, castor::Path > m_files;
		ProgramModule m_module;
		ashes::PipelineShaderStageCreateInfoArray m_states;
	};

	C3D_API SpirVShader const & compileShader( RenderDevice const & device
		, ShaderModule & shaderModule );
	C3D_API SpirVShader const & compileShader( RenderSystem & renderSystem
		, ShaderModule & shaderModule );
	C3D_API SpirVShader const & compileShader( RenderDevice const & device
		, ProgramModule & shaderModule
		, ast::EntryPointConfig const & entryPoint );
	C3D_API SpirVShader const & compileShader( RenderSystem & renderSystem
		, ProgramModule & shaderModule
		, ast::EntryPointConfig const & entryPoint );
	C3D_API ashes::PipelineShaderStageCreateInfoArray makeProgramStates( RenderDevice const & device
		, ProgramModule & programModule
		, ashes::Optional< ashes::SpecializationInfo > const & specialization = ashes::nullopt );
	C3D_API ast::ShaderStage getShaderStage( RenderDevice const & device
		, VkShaderStageFlagBits value );
	C3D_API VkShaderStageFlagBits getVkShaderStage( ast::ShaderStage value );
	C3D_API VkShaderStageFlagBits getVkShaderStage( ast::EntryPoint value );
	C3D_API ast::EntryPoint getEntryPointType( RenderDevice const & device
		, VkShaderStageFlagBits value );

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( ashes::Device const & device
		, VkShaderStageFlagBits stage
		, SpirVShader const & code
		, castor::String const & name
		, castor::String const & mainFuncName = cuT( "main" )
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt )
	{
		auto shaderModule = device.createShaderModule( castor::toUtf8( name ) + "ShdMod_" + ashes::getName( stage )
			, code.spirv );
		return ashes::PipelineShaderStageCreateInfo{ 0u
			, stage
			, castor::move( shaderModule )
			, castor::toUtf8( mainFuncName )
			, castor::move( specialization ) };
	}

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( RenderDevice const & device
		, ShaderModule & shaderModule
		, castor::String const & mainFuncName = cuT( "main" )
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt )
	{
		return makeShaderState( *device
			, shaderModule.stage
			, compileShader( device, shaderModule )
			, shaderModule.name
			, mainFuncName
			, castor::move( specialization ) );
	}

	struct ParticleSystemContext;

	struct ProgramContext
	{
		ParticleSystemContext * particleSystem{};
		castor::String name{};
		ShaderProgramRPtr shaderProgram{};
		VkShaderStageFlagBits shaderStage{};
	};
}

#endif
