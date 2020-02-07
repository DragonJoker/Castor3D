/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderProgram_H___
#define ___C3D_ShaderProgram_H___

#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ShaderWriter/Shader.hpp>

#include <CastorUtils/Design/OwnedBy.hpp>

#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>

namespace castor3d
{
	template< typename KeyType >
	struct ElementProducer< ShaderProgram, KeyType >
	{
		using Type = std::function< std::shared_ptr< ShaderProgram >( KeyType const & ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.2
	\date		24/01/2011
	\~english
	\brief		Base shader program implementation, used to share functions through the supported shader languages.
	\~french
	\brief		Implémentation de base d'un programme de shader, utilisé afin d'exposer les fonctions communes aux langages de shader.
	*/
	class ShaderProgram
		: public castor::OwnedBy< RenderSystem >
		, public std::enable_shared_from_this< ShaderProgram >
	{
		friend class castor::TextWriter< castor3d::ShaderProgram >;

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		ShaderProgram loader.
		\~french
		\brief		Loader de ShaderProgram.
		*/
		class TextWriter
			: public castor::TextWriter< ShaderProgram >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( castor::String const & tabs, castor::String const & name = cuT( "shader_program" ) );
			/**
			 *\~english
			 *\brief			Writes a ShaderProgram into a text file.
			 *\param[in]		program	The ShaderProgram.
			 *\param[in,out]	file		The file.
			 *\~french
			 *\brief			Ecrit ShaderProgram dans un fichier texte.
			 *\param[in]		program	Le ShaderProgram.
			 *\param[in,out]	file		Le fichier.
			 */
			C3D_API bool operator()( ShaderProgram const & program, castor::TextFile & file )override;

		private:
			castor::String m_name;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderSystem	The RenderSystem instance.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderSystem	L'instance du RenderSystem.
		 */
		C3D_API explicit ShaderProgram( RenderSystem & renderSystem );
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
		 *\~french
		 *\brief		Initialise le programme.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans the program up.
		 *\~french
		 *\brief		Nettoie le programme.
		 */
		C3D_API void cleanup();
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

	public:
		/**@name Attributes */
		//@{

		//!\~english	Name of the position attribute.
		//!\~french		Nom de l'attribut position.
		C3D_API static const castor::String Position;
		//!\~english	Name of the normal attribute.
		//!\~french		Nom de l'attribut normale.
		C3D_API static const castor::String Normal;
		//!\~english	Name of the tangent attribute.
		//!\~french		Nom de l'attribut tangente.
		C3D_API static const castor::String Tangent;
		//!\~english	Name of the bitangent attribute.
		//!\~french		Nom de l'attribut bitangente.
		C3D_API static const castor::String Bitangent;
		//!\~english	Name of the texture attribute.
		//!\~french		Nom du de l'attribut texture.
		C3D_API static const castor::String Texture;
		//!\~english	Name of the colour attribute.
		//!\~french		Nom du de l'attribut couleur.
		C3D_API static const castor::String Colour;
		//!\~english	Name of the position attribute for per-vertex animations.
		//!\~french		Nom de l'attribut position pour les animations par sommet.
		C3D_API static const castor::String Position2;
		//!\~english	Name of the normal attribute for per-vertex animations.
		//!\~french		Nom de l'attribut normale pour les animations par sommet.
		C3D_API static const castor::String Normal2;
		//!\~english	Name of the tangent attribute for per-vertex animations.
		//!\~french		Nom de l'attribut tangente pour les animations par sommet.
		C3D_API static const castor::String Tangent2;
		//!\~english	Name of the bitangent attribute for per-vertex animations.
		//!\~french		Nom de l'attribut bitangente pour les animations par sommet.
		C3D_API static const castor::String Bitangent2;
		//!\~english	Name of the texture attribute for per-vertex animations.
		//!\~french		Nom du de l'attribut texture pour les animations par sommet.
		C3D_API static const castor::String Texture2;
		//!\~english	Name of the colour attribute for per-vertex animations.
		//!\~french		Nom du de l'attribut couleur pour les animations par sommet.
		C3D_API static const castor::String Colour2;
		//!\~english	Name of the text overlay texture attribute.
		//!\~french		Nom du de l'attribut texture pour les incrustations texte.
		C3D_API static const castor::String Text;
		//!\~english	Name of the first bones ID attribute.
		//!\~french		Nom du premier attribut d'ID des bones.
		C3D_API static const castor::String BoneIds0;
		//!\~english	Name of the second bones ID attribute.
		//!\~french		Nom du second attribut d'ID des bones.
		C3D_API static const castor::String BoneIds1;
		//!\~english	Name of the first bones weight attribute.
		//!\~french		Nom du premier attribut de poids des bones.
		C3D_API static const castor::String Weights0;
		//!\~english	Name of the second bones weight attribute.
		//!\~french		Nom du second attribut de poids des bones.
		C3D_API static const castor::String Weights1;
		//!\~english	Name of the instance transform attribute.
		//!\~french		Nom de l'attribut d'instance de transformation.
		C3D_API static const castor::String Transform;
		//!\~english	Name of the instance material index attribute.
		//!\~french		Nom de l'attribut d'instance d'indice de matériau.
		C3D_API static const castor::String Material;

		//@}
		/**@name Scene */
		//@{

		//!\~english	Name of the lights frame variable.
		//!\~french		Nom de la frame variable contenant les lumières.
		C3D_API static const castor::String Lights;

		//@}
		/**@name Textures */
		//@{

		//!\~english	Name of the albedo texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur albedo.
		C3D_API static const castor::String MapAlbedo;
		//!\~english	Name of the diffuse texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur diffuse.
		C3D_API static const castor::String MapDiffuse;
		//!\~english	Name of the specular texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur spéculaire.
		C3D_API static const castor::String MapSpecular;
		//!\~english	Name of the roughness texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de rugosité.
		C3D_API static const castor::String MapRoughness;
		//!\~english	Name of the emissive texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur émissive.
		C3D_API static const castor::String MapEmissive;
		//!\~english	Name of the emissive texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de normales.
		C3D_API static const castor::String MapNormal;
		//!\~english	Name of the opacity texture frame variable.
		//!\~french		Nom de la frame variable contenant texture d'opacité.
		C3D_API static const castor::String MapOpacity;
		//!\~english	Name of the shininess texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'exposante.
		C3D_API static const castor::String MapGloss;
		//!\~english	Name of the metallic texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de "métallosité".
		C3D_API static const castor::String MapMetallic;
		//!\~english	Name of the height texture frame variable.
		//!\~french		Nom de la frame variable contenant texture de hauteur.
		C3D_API static const castor::String MapHeight;
		//!\~english	Name of the ambient occlusion texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'occlusion ambiante.
		C3D_API static const castor::String MapAmbientOcclusion;
		//!\~english	Name of the transmittance texture frame variable.
		//!\~french		Nom de la frame variable contenant texture de transmission.
		C3D_API static const castor::String MapTransmittance;
		//!\~english	Name of the environment texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'environnement.
		C3D_API static const castor::String MapEnvironment;
		//!\~english	Name of the irradiance texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'irradiance.
		C3D_API static const castor::String MapIrradiance;
		//!\~english	Name of the prefiltered map texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture préfiltrée.
		C3D_API static const castor::String MapPrefiltered;
		//!\~english	Name of the BRDF texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture BRDF.
		C3D_API static const castor::String MapBrdf;
		//!\~english	Name of the overlay text image frame variable.
		//!\~french		Nom de la frame variable contenant l'image de texte pour les overlays.
		C3D_API static const castor::String MapText;

		//@}

	protected:
		std::map< VkShaderStageFlagBits, castor::Path > m_files;
		std::map< VkShaderStageFlagBits, ShaderModule > m_modules;
		ashes::PipelineShaderStageCreateInfoArray m_states;
	};

	C3D_API SpirVShader compileShader( RenderDevice const & device
		, ShaderModule const & module );
	C3D_API SpirVShader compileShader( RenderSystem const & renderSystem
		, ShaderModule const & module );

	inline ashes::PipelineShaderStageCreateInfo makeShaderState( RenderDevice const & device
		, VkShaderStageFlagBits stage
		, SpirVShader code
		, std::string const & name
		, std::string mainFuncName = "main"
		, ashes::Optional< ashes::SpecializationInfo > specialization = ashes::nullopt )
	{
		auto module = device->createShaderModule( code.spirv );
		setDebugObjectName( device, *module, name + "ShdMod" + "_" + ashes::getName( stage ) );
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
		return makeShaderState( device
			, shaderModule.stage
			, compileShader( device, shaderModule )
			, shaderModule.name
			, std::move( mainFuncName )
			, std::move( specialization ) );
	}
}

#endif
