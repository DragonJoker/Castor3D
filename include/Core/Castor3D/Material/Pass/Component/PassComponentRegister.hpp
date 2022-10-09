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
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API explicit PassComponentRegister( Engine & engine );
		C3D_API ~PassComponentRegister();
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
		C3D_API std::vector< shader::PassComponentsShaderPtr > getComponentsShaders( TextureFlags const & texturesFlags
			, ComponentModeFlags filter
			, std::vector< UpdateComponent > & updateComponents )const;
		C3D_API std::vector< shader::PassComponentsShaderPtr > getComponentsShaders( PipelineFlags const & flags
			, ComponentModeFlags filter
			, std::vector< UpdateComponent > & updateComponents )const;
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
		 *\param[in,out]	texConfig	Used to determine if a map component is needed.
		 *\param[in]		pass		The pass.
		 *\~french
		 *\brief			Ajoute ou supprime les composants de texture de la passe, selon s'ils sont nécessaires ou pas.
		 *\param[in,out]	texConfig	Utilisé pour déterminer si un composant de texture est nécessaire.
		 *\param[in]		pass		La passe.
		 */
		C3D_API void updateMapComponents( TextureConfiguration const & texConfig
			, Pass & result );
		C3D_API void fillChannels( TextureFlags const & flags
			, SceneFileContext & parsingContext );

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
		C3D_API uint32_t registerComponent( castor::String const & componentType
			, ParsersFiller createParsers
			, SectionsFiller createSections
			, CreateMaterialShader createMaterialShader
			, ComponentData data );
		C3D_API void unregisterComponent( castor::String const & componentType );
		C3D_API uint32_t getNameId( castor::String const & componentType )const;
		C3D_API PassComponentsBitset getPassComponentsBitset( Pass const * pass = nullptr )const;

		template< typename ComponentT >
		uint32_t registerComponent( ParsersFiller pcreateParsers = &ComponentT::createParsers
			, SectionsFiller pcreateSections = &ComponentT::createSections
			, CreateMaterialShader pcreateMaterialShader = &ComponentT::createMaterialShader
			, ZeroBuffer pzeroBuffer = &ComponentT::zeroBuffer
			, FillRemapMask pfillRemapMask = &ComponentT::fillRemapMask
			, WriteTextureConfig pwriteTextureConfig = &ComponentT::writeTextureConfig
			, NeedsMapComponent pneedsMapComponent = &ComponentT::needsMapComponent
			, CreateMapComponent pcreateMapComponent = &ComponentT::createMapComponent
			, IsComponentNeeded pisComponentNeeded = &ComponentT::isComponentNeeded
			, CreateComponentsShader pcreateComponentsShader = &ComponentT::createComponentsShader
			, std::function< UpdateComponent() > getUpdateComponent = &ComponentT::getUpdateComponent )
		{
			return registerComponent( ComponentT::TypeName
				, pcreateParsers
				, pcreateSections
				, pcreateMaterialShader
				, ComponentData{ pzeroBuffer
					, pfillRemapMask
					, pwriteTextureConfig
					, pneedsMapComponent
					, pcreateMapComponent
					, pisComponentNeeded
					, pcreateComponentsShader
					, getUpdateComponent() } );
		}
		/**@}*/

	private:
		struct Component
		{
			std::string name;
			ComponentData data{};
		};
		using ComponentIds = std::map< uint32_t, Component >;

	private:
		uint32_t getNextId();
		void registerComponent( uint32_t id
			, castor::String const & componentType
			, ParsersFiller createParsers
			, SectionsFiller createSections
			, CreateMaterialShader createMaterialShader
			, ComponentData data );
		void unregisterComponent( uint32_t id );
		void reorderBuffer();

		using FillMaterialType = std::function< void ( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits
			, uint32_t & padIndex ) >;

	private:
		ComponentIds m_ids;
		std::map< uint32_t, shader::PassMaterialShaderPtr > m_materialShaders;
		castor::UInt32StrMap m_channels;
		ChannelFillers m_channelsFillers;
		bool m_pauseOrder{ true };
		std::vector< uint32_t > m_bufferOrder;
		std::vector< shader::PassMaterialShader * > m_bufferShaders;
		std::vector< FillMaterialType > m_fillMaterial;
		VkDeviceSize m_bufferStride{};
	};
}

#endif
