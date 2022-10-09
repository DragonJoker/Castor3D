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
			C3D_API virtual ~PassShader() = default;
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
			C3D_API virtual void fillComponents( sdw::type::BaseStruct & components
				, Materials const & materials
				, Material const * material
				, sdw::StructInstance const * surface )const
			{
			}

			C3D_API virtual void fillComponentsInits( sdw::type::BaseStruct & components
				, Materials const & materials
				, Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const
			{
			}

			C3D_API virtual void applyComponents( TextureFlags const & texturesFlags
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::Vec4 const & sampled
				, BlendComponents const & components )const
			{
			}

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
			explicit PassMaterialShader( MemChunk v )
				: m_chunk{ std::move( v ) }
			{
			}

			C3D_API virtual void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const
			{
			}

			C3D_API virtual void updateMaterial( sdw::Vec3 const & albedo
				, sdw::Vec4 const & spcRgh
				, sdw::Vec4 const & colMtl
				, Material & material )const
			{
			}

			C3D_API void setMaterialChunk( MemChunk v )
			{
				m_chunk = std::move( v );
			}

			C3D_API MemChunk const & getMaterialChunk()const
			{
				return m_chunk;
			}

		private:
			MemChunk m_chunk{};
		};
	}

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

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers )
		{
		}

		C3D_API static void createSections( castor::StrUInt32Map & sections )
		{
		}

		C3D_API static void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer )
		{
		}

		C3D_API static void fillRemapMask( uint32_t maskValue
			, TextureConfiguration & configuration )
		{
		}

		C3D_API static bool writeTextureConfig( TextureConfiguration const & configuration
			, castor::String const & tabs
			, castor::StringStream & file )
		{
			return true;
		}

		C3D_API static bool needsMapComponent( TextureConfiguration const & configuration )
		{
			return false;
		}

		C3D_API static void createMapComponent( Pass & pass
			, std::vector< PassComponentUPtr > & result )
		{
		}

		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter )
		{
			return true;
		}

		C3D_API static UpdateComponent getUpdateComponent()
		{
			return nullptr;
		}

		C3D_API static shader::PassComponentsShaderPtr createComponentsShader()
		{
			return nullptr;
		}

		C3D_API static shader::PassMaterialShaderPtr createMaterialShader()
		{
			return nullptr;
		}
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
		castor::String const & getType()const
		{
			return m_type;
		}

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
