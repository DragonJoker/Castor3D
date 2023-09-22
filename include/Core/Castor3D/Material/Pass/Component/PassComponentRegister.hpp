/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassComponentRegister_H___
#define ___C3D_PassComponentRegister_H___

#include "ComponentModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Design/DynamicBitset.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <map>
#include <unordered_map>

namespace castor3d
{
	class PassComponentRegister
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\name
		 *	Construction / Destruction.
		 */
		/**@{*/
		C3D_API explicit PassComponentRegister( Engine & engine );
		C3D_API ~PassComponentRegister();
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Pass type registration.
		 *\~french
		 *\name
		 *	Enregistrement de type de passe.
		 */
		/**@{*/
		C3D_API PassComponentCombine registerPassComponentCombine( Pass const & pass );
		C3D_API PassComponentCombineID registerPassComponentCombine( PassComponentCombine & combine );
		C3D_API PassComponentCombineID getPassComponentCombineID( PassComponentCombine const & combine )const;
		C3D_API TextureCombineID getTextureCombineID( TextureCombine const & combine )const;
		C3D_API PassComponentCombine getPassComponentCombine( Pass const & pass )const;
		C3D_API PassComponentCombine getPassComponentCombine( PassComponentCombineID id )const;
		C3D_API TextureCombine getTextureCombine( Pass const & pass )const;
		C3D_API TextureCombine getTextureCombine( TextureCombineID id )const;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Shader material components handling.
		 *\~french
		 *\name
		 *	Gestion des composants de matériau shader.
		 */
		/**@{*/
		/**
		 *\~english
		 *\brief			Fills the pass buffer with initialisation values.
		 *\param[in,out]	buffer	The pass buffer.
		 *\param[in]		pass	The pass, to prevent initialising enabled components' data.
		 *\~french
		 *\brief			Remplit le pass buffer avec les valeurs d'initialisation.
		 *\param[in,out]	buffer	Le pass buffer.
		 *\param[in]		pass	La passe, pour ne pas initialiser les données de ses composants actifs.
		 */
		C3D_API void fillBuffer( Pass const & pass
			, PassBuffer & buffer )const;
		C3D_API shader::PassMaterialShader * getMaterialShader( castor::String const & componentType )const;
		C3D_API void fillMaterial( sdw::type::BaseStruct & material
			, sdw::expr::ExprList & inits
			, uint32_t padIndex )const;

		auto const & getMaterialShaders()const
		{
			return m_bufferShaders;
		}

		VkDeviceSize getPassBufferStride()const
		{
			return m_bufferStride;
		}
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Shader output components handling.
		 *\~french
		 *\name
		 *	Gestion des composants de sortie des shaders.
		 */
		/**@{*/
		C3D_API std::vector< shader::PassComponentsShaderPtr > getComponentsShaders( TextureCombine const & texturesFlags
			, ComponentModeFlags filter
			, std::vector< UpdateComponent > & updateComponents
			, std::vector< FinishComponent > & finishComponents )const;
		C3D_API std::vector< shader::PassComponentsShaderPtr > getComponentsShaders( PipelineFlags const & flags
			, ComponentModeFlags filter
			, std::vector< UpdateComponent > & updateComponents
			, std::vector< FinishComponent > & finishComponents )const;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Base components handling.
		 *\~french
		 *\name
		 *	Gestion des composants de base.
		 */
		/**@{*/
		C3D_API PassComponentCombine filterComponentFlags( ComponentModeFlags filter
			, PassComponentCombine const & combine )const;
		C3D_API bool hasOpacity( PipelineFlags const & flags )const;
		C3D_API bool needsEnvironmentMapping( PassComponentCombineID combineID )const;
		C3D_API bool hasDeferredLighting( PassComponentFlagsSet const & combineID )const;

		PassComponentFlag getAlphaBlendingFlag()const
		{
			return m_alphaBlendingFlag;
		}

		PassComponentFlag getAlphaTestFlag()const
		{
			return m_alphaTestFlag;
		}

		PassComponentFlag getTransmissionFlag()const
		{
			return m_transmissionFlag;
		}

		PassComponentFlag getParallaxOcclusionMappingOneFlag()const
		{
			return m_parallaxOcclusionMappingOneFlag;
		}

		PassComponentFlag getParallaxOcclusionMappingRepeatFlag()const
		{
			return m_parallaxOcclusionMappingRepeatFlag;
		}
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Texture components handling.
		 *\~french
		 *\name
		 *	Gestion des composants de texture.
		 */
		/**@{*/
		/**
		 *\~english
		 *\brief			Adds or removes texture components, whether they are needed or not, from given pass.
		 *\param[in]		texConfigs	Used to determine if a map component is needed.
		 *\param[in,out]	result		The pass.
		 *\~french
		 *\brief			Ajoute ou supprime les composants de texture de la passe, selon s'ils sont nécessaires ou pas.
		 *\param[in]		texConfigs	Utilisé pour déterminer si un composant de texture est nécessaire.
		 *\param[in,out]	result		La passe.
		 */
		C3D_API void updateMapComponents( std::vector< TextureFlagConfiguration > const & texConfigs
			, Pass & result );
		/**
		 *\~english
		 *\brief		Writes the texture configuration for this component in a scene file.
		 *\param[in]	configuration	Holds the texture configuration data for this component.
		 *\param[in]	tabs			The current indentation in the output file.
		 *\param[in]	file			The output file.
		 *\~french
		 *\brief		Ecrit la configuration de texture pour ce composant dans un fichier de scène.
		 *\param[in]	configuration	Contient les données de configuration de texture de ce composant.
		 *\param[in]	tabs			Le niveau d'indentation dans le fichier de sortie.
		 *\param[in]	file			Le fichier de sortie.
		 */
		C3D_API bool writeTextureConfig( TextureConfiguration const & configuration
			, castor::String const & tabs
			, castor::StringStream & file )const;
		C3D_API void fillChannels( PassComponentTextureFlag const & flags
			, SceneFileContext & parsingContext );
		C3D_API TextureCombine filterTextureFlags( ComponentModeFlags filter
			, TextureCombine const & combine )const;
		C3D_API PassComponentTextureFlag getColourMapFlags()const;
		C3D_API PassComponentTextureFlag getOpacityMapFlags()const;
		C3D_API PassComponentTextureFlag getNormalMapFlags()const;
		C3D_API PassComponentTextureFlag getHeightMapFlags()const;
		C3D_API PassComponentTextureFlag getOcclusionMapFlags()const;
		C3D_API void fillTextureConfiguration( PassComponentTextureFlag const & flags
			, TextureConfiguration & result )const;
		C3D_API bool hasTexcoordModif( PassComponentTextureFlag const & flag
			, PipelineFlags const * flags )const;
		C3D_API std::map< uint32_t, PassComponentTextureFlag > getTexcoordModifs( PipelineFlags const & flags )const;
		C3D_API std::map< uint32_t, PassComponentTextureFlag > getTexcoordModifs( TextureCombine const & combine )const;

		castor::UInt32StrMap const & getTextureChannels()const
		{
			return m_channels;
		}
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Components registration.
		 *\~french
		 *\name
		 *	Enregistrement des composants.
		 */
		/**@{*/
		C3D_API PassComponentID registerComponent( castor::String const & componentType
			, PassComponentPluginUPtr componentPlugin );
		C3D_API void unregisterComponent( castor::String const & componentType );
		C3D_API PassComponentID getNameId( castor::String const & componentType )const;
		C3D_API PassComponentPlugin const & getPlugin( PassComponentID componentId )const;

		PassComponentPlugin const & getPlugin( castor::String const & componentType )const
		{
			return getPlugin( getNameId( componentType ) );
		}

		template< typename ComponentT >
		PassComponentPlugin const & getPlugin()const
		{
			return getPlugin( getNameId( ComponentT::TypeName ) );
		}

		template< typename ComponentT >
		PassComponentID registerComponent( CreatePassComponentPlugin createPlugin = &ComponentT::createPlugin )
		{
			return registerComponent( ComponentT::TypeName
				, createPlugin( *this ) );
		}

		auto begin()const noexcept
		{
			return m_registered.begin();
		}

		auto end()const noexcept
		{
			return m_registered.end();
		}
		/**@}*/

	private:
		struct Component
		{
			PassComponentID id{};
			std::string name{};
			PassComponentPluginUPtr plugin{};
		};
		using Components = std::vector< Component >;

	private:
		Component & getNextId();
		void registerComponent( Component & componentDesc
			, castor::String const & componentType
			, PassComponentPluginUPtr componentPlugin );
		void unregisterComponent( PassComponentID id );
		void reorderBuffer();
		void fillPassComponentCombine( PassComponentCombine & combine );

		using FillMaterialType = std::function< void ( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits
			, uint32_t & padIndex ) >;

	private:
		Components m_registered;
		std::map< PassComponentID, shader::PassMaterialShaderPtr > m_materialShaders;
		castor::UInt32StrMap m_channels;
		ChannelFillers m_channelsFillers;
		bool m_pauseOrder{ true };
		std::vector< PassComponentID > m_bufferOrder;
		std::vector< shader::PassMaterialShader * > m_bufferShaders;
		std::vector< FillMaterialType > m_fillMaterial;
		VkDeviceSize m_bufferStride{};
		mutable std::vector< PassComponentCombine > m_componentCombines{};
		PassComponentFlag m_alphaBlendingFlag{};
		PassComponentFlag m_alphaTestFlag{};
		PassComponentFlag m_transmissionFlag{};
		PassComponentFlag m_parallaxOcclusionMappingOneFlag{};
		PassComponentFlag m_parallaxOcclusionMappingRepeatFlag{};
		PassComponentFlag m_deferredDiffuseLightingFlag{};
	};
}

#endif
