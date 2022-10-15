/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassComponent_H___
#define ___C3D_PassComponent_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"

#include <CastorUtils/Graphics/RgbColour.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <unordered_set>
#pragma warning( pop )

namespace castor3d
{
	namespace shader
	{
		struct PassShader
		{
			PassShader() = default;
			PassShader( PassShader const & ) = delete;
			PassShader & operator=( PassShader const & ) = delete;
			C3D_API PassShader( PassShader && ) = default;
			C3D_API PassShader & operator=( PassShader && ) = default;
			C3D_API virtual ~PassShader() = default;
			/**
			*\~english
			*\brief
			*	Updates the values needed for lighting (specular, metalness, roughness and colour).
			*\param[in] components
			*	The components used as source.
			*\param[in] surface
			*	The surface also used as source.
			*\param[out] spcRgh
			*	Receives Specular (RGB) and Roughness (A).
			*\param[out] colMtl
			*	Receives Colour (RGB) and Metalness (A).
			*\~french
			*\brief		Met à jour les valeurs nécessaires à l'éclairage (specular, metalness, roughness et colour).
			*\param[in] components
			*	Les composants source.
			*\param[in] surface
			*	La surface source.
			*\param[out] spcRgh
			*	Reçoit la Specular (RGB) et la Roughness (A).
			*\param[out] colMtl
			*	Reçoit la Colour (RGB) et la Metalness (A).
			*/
			C3D_API virtual void updateOutputs( sdw::StructInstance const & components
				, sdw::StructInstance const & surface
				, sdw::Vec4 & spcRgh
				, sdw::Vec4 & colMtl )const
			{
			}
		};

		struct PassComponentsShader
			: public PassShader
		{
			explicit PassComponentsShader( PassComponentPlugin const & plugin )
				: m_plugin{ plugin }
			{
			}
			/**
			*\~english
			*\brief
			*	Fills the components shader struct with the members provided by this component.
			*\param[in,out] components
			*	Receives the struct members.
			*\param[in] materials
			*	Used to check if the current render pass needs this component.
			*\param[in] surface
			*	Used to check if the surface supports needed data for this component.
			*\~french
			*\brief
			*	Remplit la structure shader de composants avec les membres fournis par ce composant.
			*\param[in,out] components
			*	Reçoit les membres de la structure.
			*\param[in] materials
			*	Utilisé pour vérifier si la passe de rendu courante a besoin de ce composant.
			*\param[in] surface
			*	Utilisés pour vérifier si la surface supporte les données nécessaires pour ce composant.
			*/
			C3D_API virtual void fillComponents( sdw::type::BaseStruct & components
				, Materials const & materials
				, sdw::StructInstance const * surface )const
			{
			}
			/**
			*\~english
			*\brief
			*	Fills the components shader struct members initialiser with the ones provided by this component.
			*\remarks
			*	The initialisers order and count must match the ones in fillComponent.
			*\param[in] components
			*	Receives the struct members.
			*\param[in] materials
			*	Used to check if the current render pass needs this component.
			*\param[in] material
			*	Used to retrieve needed data for this component.
			*\param[in] surface
			*	Used to retrieve needed data for this component.
			*\param[in,out] inits
			*	Receives the initialisers.
			*\~french
			*\brief
			*	Remplit les initialiseurs de la structure shader de composants avec les ceux fournis par ce composant.
			*\remarks
			*	Le nombre et l'ordre des initialiseurs doit correspondre à celui défini par fillComponent.
			*\param[in] components
			*	Reçoit les membres de la structure.
			*\param[in] materials
			*	Utilisé pour vérifier si la passe de rendu courante a besoin de ce composant.
			*\param[in] material
			*	Utilisé pour récupérer les données pour ce composant.
			*\param[in] surface
			*	Utilisé pour récupérer les données pour ce composant.
			*\param[in,out] inits
			*	Reçoit les initialiseurs
			*/
			C3D_API virtual void fillComponentsInits( sdw::type::BaseStruct const & components
				, Materials const & materials
				, Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const
			{
			}
			/**
			*\~english
			*\brief
			*	Use this component ta alter texture coordinates.
			*\param[in] flags
			*	Used to check if the render pass is configured so the component is usable.
			*\param[in] config
			*	Used to say if the texture has the needed configuration for this component.
			*\param[in] imgCompConfig
			*	The current image component(s) to use.
			*\param[in] map
			*	The texture to use.
			*\param[in] texCoords
			*	The 3D texture coordinates.
			*\param[in] texCoord
			*	The 2D texture coordinates.
			*\param[in] components
			*	Contains the component members.
			*\~french
			*\brief
			*	Utilise ce composant pour altérer les coordonnées de texture.
			*\param[in] flags
			*	Utilisé pour vérifier si la passe de rendu est configurée pour que le composant soit utilisable.
			*\param[in] config
			*	Utilisé pour dire si la texture a la configuration pour ce composant.
			*\param[in] imgCompConfig
			*	Les composantes de l'image à utiliser.
			*\param[in] map
			*	La texture à utiliser.
			*\param[in] texCoords
			*	Les coordonnées de texture 3D.
			*\param[in] texCoord
			*	Les coordonnées de texture 2D.
			*\param[in] components
			*	Contient les membres du composant.
			*/
			C3D_API virtual void computeTexcoord( PipelineFlags const & flags
				, TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 & texCoords
				, sdw::Vec2 & texCoord
				, BlendComponents & components )const
			{
			}
			/**
			*\~english
			*\brief
			*	Use this component ta alter texture coordinates.
			*\param[in] flags
			*	Used to check if the render pass is configured so the component is usable.
			*\param[in] config
			*	Used to say if the texture has the needed configuration for this component.
			*\param[in] imgCompConfig
			*	The current image component(s) to use.
			*\param[in] map
			*	The texture to use.
			*\param[in] texCoords
			*	The 3D texture coordinates.
			*\param[in] texCoord
			*	The 2D texture coordinates.
			*\param[in] components
			*	Contains the component members.
			*\~french
			*\brief
			*	Utilise ce composant pour altérer les coordonnées de texture.
			*\param[in] flags
			*	Utilisé pour vérifier si la passe de rendu est configurée pour que le composant soit utilisable.
			*\param[in] config
			*	Utilisé pour dire si la texture a la configuration pour ce composant.
			*\param[in] imgCompConfig
			*	Les composantes de l'image à utiliser.
			*\param[in] map
			*	La texture à utiliser.
			*\param[in] texCoords
			*	Les coordonnées de texture 3D.
			*\param[in] texCoord
			*	Les coordonnées de texture 2D.
			*\param[in] components
			*	Contient les membres du composant.
			*/
			C3D_API virtual void computeTexcoord( PipelineFlags const & flags
				, TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::CombinedImage2DRgba32 const & map
				, DerivTex & texCoords
				, DerivTex & texCoord
				, BlendComponents & components )const
			{
			}
			/**
			*\~english
			*\brief
			*	Fills this component's values with the data retrieved from a texture.
			*\param[in] combine
			*	Used to check if the component's textures are enabled.
			*\param[in] flags
			*	Used to check if the render pass is configured so the component is usable.
			*\param[in] config
			*	Used to say if the texture has the needed configuration for this component.
			*\param[in] sampled
			*	The data retrieved from the texture.
			*\param[in] components
			*	Contains the component members.
			*\~french
			*\brief
			*	Remplit les valeurs de ce composant avec des données récupérées depuis une texture.
			*\param[in] combine
			*	Utilisé pour vérifier si les textures du composant sont activées.
			*\param[in] flags
			*	Utilisé pour vérifier si la passe de rendu est configurée pour que le composant soit utilisable.
			*\param[in] config
			*	Utilisé pour dire si la texture a la configuration pour ce composant.
			*\param[in] sampled
			*	Les données récupérées depuis la texture.
			*\param[in] components
			*	Contient les membres du composant.
			*/
			C3D_API virtual void applyComponents( TextureCombine const & combine
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, BlendComponents & components )const
			{
			}
			/**
			*\~english
			*\brief
			*	Used to mix components when multiple passes are blended together.
			*\param[in] materials
			*	Used to check if the current render pass needs this component.
			*\param[in] passMultiplier
			*	The factor for the current pass.
			*\param[in,out] res
			*	Receives the blending result.
			*\param[in] src
			*	The current pass data.
			*\~french
			*\brief
			*	Utilisé pour mixer les composant quand plusieurs passes sont fusionnées ensemble.
			*\param[in] materials
			*	Utilisé pour vérifier si la passe de rendu courante a besoin de ce composant.
			*\param[in] passMultiplier
			*	Le facteur de la passe courant.
			*\param[in,out] res
			*	Reçoit le résultat du blending.
			*\param[in] src
			*	Les données de la passe courante.
			*/
			C3D_API virtual void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, BlendComponents & res
				, BlendComponents const & src )const
			{
			}
			/**
			*\~english
			*	Adjusts the component data after textures have been parsed.
			*\param[in] combine
			*	Used to check if the component's textures are enabled.
			*\param[in,out] components
			*	Contains the component members.
			*\~french
			*	Ajuste les données du composant après que les textures ont été traitées.
			*\param[in] combine
			*	Utilisé pour vérifier si les textures du composant sont activées.
			*\param[in,out] components
			*	Contient les membres du composant.
			*/
			C3D_API virtual void updateComponent( TextureCombine const & combine
				, shader::BlendComponents & components )const
			{
			}
			/**
			*\~english
			*	Lets the component sample the map.
			*\param[in] map
			*	The texture.
			*\param[in] texCoords
			*	The texture coordinates.
			*\~french
			*	Laisse le composant échantillonner la texture.
			*\param[in] map
			*	La texture.
			*\param[in] texCoords
			*	Les coordonnées de texture.
			*/
			C3D_API virtual sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords )const
			{
				return map.sample( texCoords.xy() );
			}
			/**
			*\~english
			*	Lets the component sample the map.
			*\param[in] map
			*	The texture.
			*\param[in] texCoords
			*	The texture coordinates.
			*\~french
			*	Laisse le composant échantillonner la texture.
			*\param[in] map
			*	La texture.
			*\param[in] texCoords
			*	Les coordonnées de texture.
			*/
			C3D_API virtual sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, shader::DerivTex const & texCoords )const
			{
				return map.grad( texCoords.uv(), texCoords.dPdx(), texCoords.dPdy() );
			}
			/**
			*\~english
			*\return
			*	\p true if the component map sampling is usable with given  \p flags.
			*\~french
			*\return
			*	\p true si l'échantillonnage de texture du composant est utilisable par rapport à \p flags.
			*/
			C3D_API virtual bool isMapSampling( PipelineFlags const & flags )const
			{
				return false;
			}
			/**
			*\name
			*	Getters.
			*/
			/**@{*/
			C3D_API PassComponentID getId()const;
			/**@}*/

		protected:
			C3D_API void applyFloatComponent( std::string const & name
				, PassComponentTextureFlag flag
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, shader::BlendComponents & components )const;
			C3D_API void applyVec3Component( std::string const & name
				, PassComponentTextureFlag flag
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, shader::BlendComponents & components )const;

		private:
			PassComponentPlugin const & m_plugin;
		};

		struct PassMaterialShader
			: public PassShader
		{
			/**
			*\~english
			*\param[in] dataSize
			*	The size of this component's data, to reserve it in PassBuffer.
			*\~french
			*\param[in] dataSize
			*	La taille des données de ce composant, pour les réserver dans le PassBuffer.
			*/
			C3D_API explicit PassMaterialShader( VkDeviceSize dataSize )
				: m_chunk{ 0, dataSize, dataSize }
			{
			}
			/**
			*\~english
			*\brief
			*	Fills the material shader struct with the members provided by this component, and their initialisers.
			*\param[in,out] type
			*	Receives the members.
			*\param[in] inits
			*	Receives the members initialisers.
			*\~french
			*\brief
			*	Remplit la structure shader de matériau avec les membres fournis par ce composant, et leurs initialiseurs.
			*\param[in] type
			*	Reçoit les membres.
			*\param[in] inits
			*	Reçoit les initialiseurs des membres.
			*/
			C3D_API virtual void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const
			{
			}
			/**
			*\~english
			*\brief
			*	Updates the material from values needed for lighting (specular, metalness, roughness and colour).
			*\param[in] colour
			*	The material colour.
			*\param[in] spcRgh
			*	Receives Specular (RGB) and Roughness (A).
			*\param[in] colMtl
			*	Receives Colour (RGB) and Metalness (A).
			*\param[out] material
			*	Receives the values necessary for this component.
			*\~french
			*\brief		Met à jour le matériau depuis les valeurs nécessaires à l'éclairage (specular, metalness, roughness et colour).
			*\param[in] colour
			*	La couleur du matériau.
			*\param[out] spcRgh
			*	La Specular (RGB) et la Roughness (A).
			*\param[out] colMtl
			*	la Colour (RGB) et la Metalness (A).
			*\param[out] material
			*	Reçoit les valeurs nécessaires pour ce composant.
			*/
			C3D_API virtual void updateMaterial( sdw::Vec3 const & colour
				, sdw::Vec4 const & spcRgh
				, sdw::Vec4 const & colMtl
				, Material & material )const
			{
			}
			/**
			*\name
			*	Getters.
			*/
			/**@{*/
			MemChunk const & getMaterialChunk()const
			{
				return m_chunk;
			}
			/**@}*/

		private:
			friend PassComponentRegister;

			void setMaterialChunk( MemChunk v )
			{
				m_chunk = std::move( v );
			}

		private:
			MemChunk m_chunk{};
		};
	}

	class PassComponentPlugin
	{
	public:
		/**
		*\name
		*	Construction / Desctruction.
		*/
		/**@{*/
		PassComponentPlugin( PassComponentPlugin const & ) = delete;
		PassComponentPlugin & operator=( PassComponentPlugin const & ) = delete;
		C3D_API PassComponentPlugin( PassComponentPlugin && ) = default;
		C3D_API PassComponentPlugin & operator=( PassComponentPlugin && ) = default;
		C3D_API virtual ~PassComponentPlugin() = default;
		/**
		*\~english
		*\param[in] pupdateComponent
		*	The function to adjust the component data after textures have been parsed.
		*\~french
		*\param[in] pupdateComponent
		*	Fonction pour ajuster les données du composant après que les textures ont été traitées.
		*/
		C3D_API explicit PassComponentPlugin( UpdateComponent pupdateComponent = nullptr )
			: updateComponent{ pupdateComponent }
		{
		}
		/**@}*/
		/**
		*\name
		*	Scene file registration.
		*/
		/**@{*/
		/**
		*\~english
		*\brief
		*	Fills the given lists with this component's parsers.
		*\param[in,out] parsers
		*	Receives the parsers.
		*\param[in,out] channelFillers
		*	Receives the texture channels and the way to fill them.
		*\~french
		*\brief
		*	Remplit les listes données avec les parsers de ce composant.
		*\param[in,out] parsers
		*	Reçoit les parsers.
		*\param[in,out] channelFillers
		*	Reçoit les canaux de textures et le moyen de les remplir.
		*/
		C3D_API virtual void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers )const
		{
		}
		/**
		*\~english
		*\brief
		*	Fills the sections list with the ones created by this component.
		*\param[in,out] sections
		*	Receives the sections.
		*\~french
		*\brief
		*	Remplit la liste de sections donnée avec les sections créées par ce composant.
		*\param[in,out] sections
		*	Reçoit les sections.
		*/
		C3D_API virtual void createSections( castor::StrUInt32Map & sections )const
		{
		}
		/**
		*\~english
		*\brief
		*	Writes the texture configuration for this component in a scene file.
		*\param[in] configuration
		*	Holds the texture configuration data for this component.
		*\param[in] tabs
		*	The current indentation in the output file.
		*\param[in] file
		*	The output file.
		*\~french
		*\brief
		*	Ecrit la configuration de texture pour ce composant dans un fichier de scène.
		*\param[in] configuration
		*	Contient les données de configuration de texture de ce composant.
		*\param[in] tabs
		*	Le niveau d'indentation dans le fichier de sortie.
		*\param[in] file
		*	Le fichier de sortie.
		*/
		C3D_API virtual bool writeTextureConfig( TextureConfiguration const & configuration
			, castor::String const & tabs
			, castor::StringStream & file )const
		{
			return true;
		}
		/**@}*/
		/**
		*\name
		*	Pass buffer zeroing (when component is not present in the pass).
		*/
		/**@{*/
		/**
		*\~english
		*\brief
		*	Fills the material buffer with zero values fot the component data.
		*\param[in] pass
		*	To find its data in the buffer.
		*\param[in] materialShader
		*	To find the component's data in the pass entry into the buffer.
		*\param[in] buffer
		*	The buffer.
		*\~french
		*\brief
		*	Remplit, dans le buffer de matériaux, les données du composant avec des valeurs à zéro.
		*\param[in] pass
		*	Pour trouver l'entrée de passe dans le buffer.
		*\param[in] materialShader
		*	Pour trouver les données du composant dans l'entrée de passe du buffer.
		*\param[in] buffer
		*	Le buffer.
		*/
		C3D_API virtual void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer )const
		{
		}
		/**@}*/
		/**
		*\name
		*	Textures handling.
		*/
		/**@{*/
		/**
		*\~english
		*\brief
		*	Creates the map component.
		*\param[in] pass
		*	To check if the configuration for the component is there.
		*\param[in,out] result
		*	Receives the created component.
		*\~french
		*\brief
		*	Crée le composant de texture.
		*\param[in] pass
		*	La passe parent.
		*\param[in,out] result
		*	Reçoit le composant créé.
		*/
		C3D_API virtual void createMapComponent( Pass & pass
			, std::vector< PassComponentUPtr > & result )const
		{
		}
		/**
		*\~english
		*\return
		*	\p true if this component processes texture data.
		*\~french
		*\return
		*	\p true si le composant traite des données de texture.
		*/
		C3D_API virtual bool isMapComponent()const
		{
			return false;
		}
		/**
		*\~english
		*\return
		*	\p true if the component replaces regular map sampling.
		*\~french
		*\return
		*	\p true si le composant remplace l'échantillonnage de texture traditionnel.
		*/
		C3D_API virtual bool replacesMapSampling()const
		{
			return false;
		}
		/**
		*\~english
		*\return
		*	\p true if this component modifies texture coordinates.
		*\~french
		*\return
		*	\p true si le composant modifie les coordonnées de texture.
		*/
		C3D_API virtual bool hasTexcoordModif( PipelineFlags const * flags )const
		{
			return false;
		}
		/**
		*\~english
		*\brief
		*	Removes from given texture flags the ones that are useless given the provided filter.
		*\~french
		*\brief
		*	Enlève des indicateurs de textures donnés ceux qui sont inutiles, par rapport au filtre donné.
		*/
		C3D_API virtual void filterTextureFlags( ComponentModeFlags filter
			, TextureCombine & texturesFlags )const
		{
		}
		/**
		*\~english
		*\brief
		*	Fills the texture configuration for use with this component.
		*\param configuration
		*	The texture configuration.
		*\param mask
		*	The mask value.
		*\~french
		*\brief
		*	Remplit une configuration de texture, pour utilisation par ce composant.
		*\param configuration
		*	La configuration de texture.
		*\param mask
		*	La valeur du masque.
		*/
		C3D_API virtual void fillTextureConfiguration( TextureConfiguration & configuration
			, uint32_t mask = 0 )const
		{
		}
		/**
		*\~english
		*\return
		*	The texture configuration with default configuration for this component.
		*\~french
		*\return
		*	La configuration de texture avec la configuration par défaut pour ce composant.
		*/
		TextureConfiguration getBaseTextureConfiguration()const
		{
			TextureConfiguration result{};
			fillTextureConfiguration( result );
			return result;
		}
		/**
		*\~english
		*	Adjust the component data after textures have been parsed.
		*\~french
		*	Ajuste les données du composant après que les textures ont été traitées.
		*/
		UpdateComponent updateComponent;
		/**
		*\~english
		*\return
		*	The texture flags for this component.
		*\~french
		*\return
		*	Les indicateurs de textures pour ce composant.
		*/
		C3D_API virtual PassComponentTextureFlag getTextureFlags()const
		{
			return 0u;
		}
		/**
		*\~english
		*\return
		*	The texture flags concatenated names.
		*\~french
		*\return
		*	Les noms concaténés des indicateurs de textures.
		*/
		C3D_API virtual castor::String getMapFlagsName( PassComponentTextureFlag const & flags )const
		{
			return castor::String{};
		}
		/**
		*\~english
		*\return
		*	The flags for the colour map.
		*\~french
		*\return
		*	Les indicateurs pour la texture de couleur.
		*/
		C3D_API virtual PassComponentTextureFlag getColourFlags()const
		{
			return 0u;
		}
		/**
		*\~english
		*\return
		*	The flags for the opacity map.
		*\~french
		*\return
		*	Les indicateurs pour la texture d'opacité.
		*/
		C3D_API virtual PassComponentTextureFlag getOpacityFlags()const
		{
			return 0u;
		}
		/**
		*\~english
		*\return
		*	The flags for the normal map.
		*\~french
		*\return
		*	Les indicateurs pour la texture de normales.
		*/
		C3D_API virtual PassComponentTextureFlag getNormalFlags()const
		{
			return 0u;
		}
		/**
		*\~english
		*\return
		*	The flags for the height map.
		*\~french
		*\return
		*	Les indicateurs pour la texture de hauteur.
		*/
		C3D_API virtual PassComponentTextureFlag getHeightFlags()const
		{
			return 0u;
		}
		/**
		*\~english
		*\return
		*	The flags for the occlusion map.
		*\~french
		*\return
		*	Les indicateurs pour la texture d'occlusion.
		*/
		C3D_API virtual PassComponentTextureFlag getOcclusionFlags()const
		{
			return 0u;
		}
		/**@}*/
		/**
		*\name
		*	Shader functions.
		*/
		/**@{*/
		/**
		*\~english
		*\brief
		*	Tells if the component's lighting shaders are needed.
		*\param[in] textures
		*	To check from texturing viewpoint.
		*\param[in,out] filter
		*	To check from render pass viewpoint.
		*\~french
		*\brief
		*	Détermine si les shaders pour l'éclairage du composant sont nécessaires.
		*\param[in] textures
		*	Pour vérifier du point de vue des textures.
		*\param[in,out] filter
		*	Pour vérifier du point de vue de la passe de rendu.
		*/
		C3D_API virtual bool isComponentNeeded( TextureCombine const & textures
			, ComponentModeFlags const & filter )const
		{
			return false;
		}
		/**
		*\~english
		*\brief
		*	Creates the component's lighting shaders.
		*\~french
		*\brief
		*	Crée les shaders pour l'éclairage du composant.
		*/
		C3D_API virtual shader::PassComponentsShaderPtr createComponentsShader()const
		{
			return nullptr;
		}
		/**
		*\~english
		*\brief
		*	Creates the component's material shaders.
		*\~french
		*\brief
		*	Crée les shaders de matériau du composant.
		*/
		C3D_API virtual shader::PassMaterialShaderPtr createMaterialShader()const
		{
			return nullptr;
		}
		/**@}*/
		/**
		*\name
		*	Getters.
		*/
		/**@{*/
		PassComponentID getId()const
		{
			return m_id;
		}
		/**@}*/

	private:
		friend class PassComponentRegister;

		void setId( PassComponentID id )
		{
			m_id = id;
		}

		PassComponentID m_id{};
	};

	struct PassComponent
		: public castor::OwnedBy< Pass >
	{
		/**
		*\name
		*	Construction / Destruction.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass	The parent pass.
		 *\param[in]	type	The component type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass	Le sous-maillage pass.
		 *\param[in]	type	Le type de composant.
		 */
		C3D_API PassComponent( Pass & pass
			, castor::String const & type );
		C3D_API virtual ~PassComponent()noexcept = default;
		/**
		 *\~english
		 *\return			Clones this component into given pass.
		 *\param[in,out]	pass	Receives the cloned component.
		 *\~french
		 *\return			Clone ce composant dans la passe donnée.
		 *\param[in,out]	pass	Reçoit le composant cloné.
		 */
		C3D_API PassComponentUPtr clone( Pass & pass )const;
		/**@}*/
		/**
		*\name
		*	Common virtual functions.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief			Writes the component content to text.
		 *\param[in]		tabs		The current tabulation level.
		 *\param[in]		folder		The resources folder.
		 *\param[in]		subfolder	The resources subfolder.
		 *\param[in,out]	file		The output file.
		 *\~french
		 *\brief			Ecrit le contenu du composant en texte.
		 *\param[in]		tabs		Le niveau actuel de tabulation.
		 *\param[in]		folder		Le dossier de ressources.
		 *\param[in]		subfolder	Le sous-dossier de ressources.
		 *\param[in,out]	file		Le fichier de sortie.
		 */
		C3D_API bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const;
		/**
		 *\~english
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer			The pass buffer.
		 *\param[in]		passTypeIndex	The pass type index.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer			Le pass buffer.
		 *\param[in]		passTypeIndex	L'indice du type de passe.
		 */
		C3D_API void fillBuffer( PassBuffer & buffer )const;
		/**
		*\~english
		*\brief
		*	PassVisitor acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de PassVisitor.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API virtual void accept( PassVisitorBase & vis )
		{
		}

		C3D_API virtual void update()
		{
		}
		/**
		*\name
		*	Images.
		*/
		/**@{*/
		/**
		*\~english
		*\brief
		*	Fills the texture configuration for use with this component.
		*\param configuration
		*	The texture configuration.
		*\param mask
		*	The mask value.
		*\~french
		*\brief
		*	Remplit une configuration de texture, pour utilisation par ce composant.
		*\param configuration
		*	La configuration de texture.
		*\param mask
		*	La valeur du masque.
		*/
		C3D_API void fillChannel( TextureConfiguration & configuration
			, uint32_t mask );
		/**
		*\~english
		*\brief
		*	PassVisitor acceptance function, for a specific texture configuration.
		*\param config
		*	The texture configuration.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de PassVisitor, pour une configuration de texture.
		*\param config
		*	La configuration de texture.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API virtual void fillConfig( TextureConfiguration & config
			, PassVisitorBase & vis )const
		{
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API virtual bool hasColour()const
		{
			return false;
		}

		C3D_API virtual castor::HdrRgbColour const & getColour()const
		{
			CU_Require( hasColour() );

			static castor::HdrRgbColour const dummy{};
			return dummy;
		}

		C3D_API virtual void setColour( castor::HdrRgbColour v )
		{
			CU_Require( hasColour() );

			static castor::HdrRgbColour dummy{};
			dummy = std::move( v );
		}

		C3D_API virtual PassFlags getPassFlags()const
		{
			return PassFlag::eNone;
		}

		castor::String const & getType()const
		{
			return m_type;
		}

		PassComponentPlugin const & getPlugin()const
		{
			return m_plugin;
		}

		PassComponentID getId()const
		{
			return m_id;
		}

		void setColour( castor::RgbColour const & v
			, float gamma = 2.2f )
		{
			setColour( castor::HdrRgbColour{ v, gamma } );
		}

		void setColour( castor::Coords3f const & v )
		{
			setColour( castor::HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}

		void setColour( castor::Point3f const & v )
		{
			setColour( castor::HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}
		/**@}*/

	private:
		/**
		 *\~english
		 *\return			Clones this component into given pass.
		 *\param[in,out]	pass	Receives the cloned component.
		 *\~french
		 *\return			Clone ce composant dans la passe donnée.
		 *\param[in,out]	pass	Reçoit le composant cloné.
		 */
		virtual PassComponentUPtr doClone( Pass & pass )const = 0;
		/**
		 *\~english
		 *\brief			Writes the component content to text.
		 *\param[in,out]	result	Receives the merged textures.
		 *\~french
		 *\brief			Ecrit le contenu du composant en texte.
		 *\param[in,out]	result		Reçoit les textures fusionnées.
		 */
		virtual bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const
		{
			return true;
		}
		/**
		 *\~english
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer			The pass buffer.
		 *\param[in]		passTypeIndex	The pass type index.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer			Le pass buffer.
		 *\param[in]		passTypeIndex	L'indice du type de passe.
		 */
		virtual void doFillBuffer( PassBuffer & buffer )const
		{
		}

	protected:
		castor::String m_type;
		PassComponentID m_id;
		PassComponentPlugin const & m_plugin;
		std::atomic_bool & m_dirty;
		shader::PassMaterialShader * m_materialShader{};
	};
}

#endif
