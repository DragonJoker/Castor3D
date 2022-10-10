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
			*	Fills this component's values with the data retrieved from a texture.
			*\param[in] texturesFlags
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
			*\param[in] texturesFlags
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
			C3D_API virtual void applyComponents( TextureFlags const & texturesFlags
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::Vec4 const & sampled
				, BlendComponents const & components )const
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
				, BlendComponents const & res
				, BlendComponents const & src )const
			{
			}
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
		*	Tells if the map component is needed in shader.
		*\param[in] configuration
		*	To check if the configuration for the component is there.
		*\~french
		*\brief
		*	Dit si le composant de texture est nécessaire dans le shader.
		*\param[in] configuration
		*	Pour vérifier si la configuration nécessaire au composant est présente.
		*/
		C3D_API virtual bool needsMapComponent( TextureConfiguration const & configuration )const
		{
			return false;
		}
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
		*	The function to adjust the component data after textures have been parsed.
		*\~french
		*	La fonction pour ajuster les données du composant après que les textures ont été traitées.
		*/
		UpdateComponent updateComponent;
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
		C3D_API virtual bool isComponentNeeded( TextureFlags const & textures
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
		 *\brief			Merges composant images, and creates the related texture data.
		 *\param[in,out]	result	Receives the merged textures data.
		 *\~french
		 *\brief			Fusionne les images du composant, et crée les données de textures correspondantes.
		 *\param[in,out]	result		Reçoit les données des textures.
		 */
		C3D_API virtual void mergeImages( TextureUnitDataSet & result )
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
		C3D_API virtual void fillChannel( TextureConfiguration & configuration
			, uint32_t mask )
		{
		}
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
			, PassVisitorBase & vis )
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
		std::atomic_bool & m_dirty;
		shader::PassMaterialShader * m_materialShader{};
	};
}

#endif
