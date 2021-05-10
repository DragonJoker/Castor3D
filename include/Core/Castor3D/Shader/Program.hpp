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
		struct CompiledShader
		{
			castor::String name;
			SpirVShader shader;
		};

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
		C3D_API virtual ~ShaderProgram();
		/**
		 *\~english
		 *\brief		Initialises the program.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le programme.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans the program up.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie le programme.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
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
		 *\brief		Retrieves the shader file.
		 *\param[in]	target	The shader object concerned.
		 *\return		The file name.
		 *\~french
		 *\brief		Récupère le fichier du shader.
		 *\param[in]	target	Le shader object concerné.
		 *\return		Le nom du fichier.
		 */
		C3D_API castor::Path getFile( VkShaderStageFlagBits target )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source file.
		 *\param[in]	target	The shader object concerned.
		 *\return		\p true if the shader object has a source file.
		 *\~french
		 *\brief		Dit si le shader a un fichier source.
		 *\param[in]	target	Le shader object concerné.
		 *\return		\p true si le shader a un fichier source.
		 */
		C3D_API bool hasFile( VkShaderStageFlagBits target )const;
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
		 *\brief		Retrieves the shader source.
		 *\param[in]	target	The shader object concerned.
		 *\return		The source code.
		 *\~french
		 *\brief		Récupère la source du shader.
		 *\param[in]	target	Le shader object concerné.
		 *\return		Le code de la source.
		 */
		C3D_API ShaderModule const & getSource( VkShaderStageFlagBits target )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source code.
		 *\param[in]	target	The shader object concerned.
		 *\return		\p true if the shader object has a source code.
		 *\~french
		 *\brief		Dit si le shader a un code source.
		 *\param[in]	target	Le shader object concerné.
		 *\return		\p true si le shader a un code source.
		 */
		C3D_API bool hasSource( VkShaderStageFlagBits target )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::PipelineShaderStageCreateInfoArray const & getStates()const
		{
			return m_states;
		}
		/**@}*/

	protected:
		std::map< VkShaderStageFlagBits, castor::Path > m_files;
		std::map< VkShaderStageFlagBits, ShaderModule > m_modules;
		std::map< VkShaderStageFlagBits, CompiledShader > m_compiled;
		ashes::PipelineShaderStageCreateInfoArray m_states;
	};

	C3D_API SpirVShader compileShader( RenderDevice const & device
		, ShaderModule const & module );
	C3D_API SpirVShader compileShader( RenderSystem const & renderSystem
		, ShaderModule const & module );

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( ashes::Device const & device
		, VkShaderStageFlagBits stage
		, SpirVShader code
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
		, ShaderModule const & shaderModule
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
