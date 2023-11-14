/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubmeshComponent_H___
#define ___C3D_SubmeshComponent_H___

#include "ComponentModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

namespace castor3d
{
	namespace shader
	{
		struct SubmeshShader
		{
			SubmeshShader() = default;
			SubmeshShader( SubmeshShader const & ) = delete;
			SubmeshShader & operator=( SubmeshShader const & ) = delete;
			C3D_API SubmeshShader( SubmeshShader && ) = default;
			C3D_API SubmeshShader & operator=( SubmeshShader && ) = default;
			C3D_API virtual ~SubmeshShader() = default;
		};

		struct SubmeshSurfaceShader
			: public SubmeshShader
		{
			/**
			*\~english
			*\brief
			*	Fills the surface shader struct with the members provided by this component, and their initialisers.
			*\param[in,out] type
			*	Receives the members.
			*\param[in] inits
			*	Receives the members initialisers.
			*\~french
			*\brief
			*	Remplit la structure shader de surface avec les membres fournis par ce composant, et leurs initialiseurs.
			*\param[in] type
			*	Reçoit les membres.
			*\param[in] inits
			*	Reçoit les initialiseurs des membres.
			*/
			C3D_API virtual void fillSurfaceType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const
			{
			}
		};

		struct SubmeshRenderShader
			: public SubmeshShader
		{
			/**
			 *\~english
			 *\brief		Retrieves the shader source matching the given flags.
			 *\param[in]	flags			The pipeline flags.
			 *\param[in]	componentsMask	The nodes pass components flags.
			 *\param[in]	builder			The shader builder.
			 *\~french
			 *\brief		Récupère le source du shader qui correspond aux indicateurs donnés.
			 *\param[in]	flags			Les indicateurs de pipeline.
			 *\param[in]	componentsMask	Les indicateurs de composants de la passe de noeuds.
			 *\param[in]	builder			Le shader builder.
			 */
			C3D_API virtual void getShaderSource( PipelineFlags const & flags
				, ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const = 0;
		};
	}

	class SubmeshComponentPlugin
	{
	public:
		/**
		*\name
		*	Construction / Desctruction.
		*/
		/**@{*/
		SubmeshComponentPlugin( SubmeshComponentPlugin const & ) = delete;
		SubmeshComponentPlugin & operator=( SubmeshComponentPlugin const & ) = delete;
		SubmeshComponentPlugin & operator=( SubmeshComponentPlugin && rhs ) = delete;
		C3D_API virtual ~SubmeshComponentPlugin() = default;
		C3D_API SubmeshComponentPlugin( SubmeshComponentPlugin && rhs ) = default;
		/**
		*\~english
		*\param[in] submeshComponents
		*	The components registrar.
		*\~french
		*\param[in] submeshComponents
		*	Le registre de composants.
		*/
		C3D_API explicit SubmeshComponentPlugin( SubmeshComponentRegister const & submeshComponents )
			: m_submeshComponents{ submeshComponents }
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
		*\~french
		*\brief
		*	Remplit les listes données avec les parsers de ce composant.
		*\param[in,out] parsers
		*	Reçoit les parsers.
		*/
		C3D_API virtual void createParsers( castor::AttributeParsers & parsers )const
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
		/**@}*/
		/**
		*\~english
		*	Creates a component for given submesh.
		*\remarks
		*	Doesn't add the component to the submesh.
		*\param[in] submesh
		*	The submesh.
		*\~french
		*\brief
		*	Crée un composant pour le submesh donné.
		*\remarks
		*	N'ajoute pas le composant au submesh.
		*\param[in] submesh
		*	Le submesh.
		*/
		C3D_API virtual SubmeshComponentUPtr createComponent( Submesh & submesh )const = 0;
		/**
		*\name
		*	Pass flags handling.
		*/
		/**@{*/
		/**
		*\~english
		*\return
		*	The pass flags for this component.
		*\~french
		*\return
		*	Les indicateurs de passe pour ce composant.
		*/
		C3D_API virtual SubmeshComponentFlag getComponentFlags()const noexcept
		{
			return makeSubmeshComponentFlag( getId() );
		}

		C3D_API virtual SubmeshComponentFlag getLineIndexFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTriangleIndexFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getPositionFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getNormalFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTangentFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getBitangentFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTexcoord0Flag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTexcoord1Flag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTexcoord2Flag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getTexcoord3Flag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getColourFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getSkinFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getMorphFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getPassMaskFlag()const noexcept
		{
			return 0u;
		}

		C3D_API virtual SubmeshComponentFlag getVelocityFlag()const noexcept
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
		C3D_API virtual bool isComponentNeeded( ComponentModeFlags const & filter )const noexcept
		{
			return false;
		}
		/**
		*\~english
		*\brief
		*	Creates the component's surface shaders.
		*\~french
		*\brief
		*	Crée les shaders pour la surface du composant.
		*/
		C3D_API virtual shader::SubmeshSurfaceShaderPtr createSurfaceShader()const
		{
			return nullptr;
		}
		/**
		*\~english
		*\brief
		*	Creates the component's render shader.
		*\~french
		*\brief
		*	Crée le shader de rendu du composant.
		*/
		C3D_API virtual shader::SubmeshSurfaceShaderPtr createRenderShader()const
		{
			return nullptr;
		}
		/**@}*/
		/**
		*\name
		*	Getters.
		*/
		/**@{*/
		SubmeshComponentID getId()const noexcept
		{
			return m_id;
		}

		SubmeshComponentRegister const & getRegister()const noexcept
		{
			return m_submeshComponents;
		}
		/**@}*/

	private:
		friend class SubmeshComponentRegister;

		void setId( SubmeshComponentID id )
		{
			m_id = id;
		}

		SubmeshComponentID m_id{};
		SubmeshComponentRegister const & m_submeshComponents;
	};

	class SubmeshComponent
		: public castor::OwnedBy< Submesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\param[in]	type	The component type.
		 *\param[in]	deps	The components this one depends on.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 *\param[in]	type	Le type de composant.
		 *\param[in]	deps	Les composants dont celui-ci dépend.
		 */
		C3D_API SubmeshComponent( Submesh & submesh
			, castor::String const & type
			, castor::StringArray deps = {} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SubmeshComponent()noexcept = default;
		/**
		 *\~english
		 *\brief		Initialises the submesh
		 *\~french
		 *\brief		Initialise le sous-maillage
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans the submesh
		 *\~french
		 *\brief		Nettoie le sous-maillage
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Uploads data on VRAM.
		 *\remarks		For host visible buffers.
		 *\~french
		 *\brief		Met les données en VRAM.
		 *\remarks		Pour les buffers host visible.
		 */
		C3D_API void upload( UploadData & uploader );
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
		C3D_API virtual bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const
		{
			return true;
		}
		/**
		*\~english
		*\brief				ConfigurationVisitorBase acceptance function.
		*\param[in,out]		vis	The ... visitor.
		*\~french
		*\brief				Fonction d'acceptation de ConfigurationVisitorBase.
		*\param[in,out]		vis	Le ... visiteur.
		*/
		C3D_API virtual void accept( ConfigurationVisitorBase & vis )
		{
		}
		/**
		 *\~english
		 *\brief			Gathers buffers that need to go in a vertex layout.
		 *\param[in]		flags			The pipeline flags.
		 *\param[in]		material		The material.
		 *\param[in,out]	buffers			Receives the buffers.
		 *\param[in,out]	offsets			Receives the buffers offsets.
		 *\param[in,out]	layouts			Receives the vertex layouts.
		 *\param[in,out]	currentBinding	The current buffer binding.
		 *\param[in,out]	currentLocation	The current attribute location.
		 *\~french
		 *\brief		Récupère les tampons qui doivent aller dans un vertex layout.
		 *\param[in]		flags			Les flags de pipeline.
		 *\param[in]		material		Les matériau.
		 *\param[in,out]	buffers			Reçoit les buffers.
		 *\param[in,out]	offsets			Reçoit les offsets dans les buffers.
		 *\param[in,out]	layouts			Reçoit les vertex layouts.
		 *\param[in,out]	currentBinding	L'index de binging actuel des buffers.
		 *\param[in,out]	currentLocation	La position actuelle des attributs.
		 */
		C3D_API virtual void gather( PipelineFlags const & flags
			, MaterialObs material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
			, uint32_t & currentBinding
			, uint32_t & currentLocation ) = 0;
		/**
		 *\~english
		 *\return		The program flags.
		 *\param[in]	material	The material.
		 *\~french
		 *\return		Les indicateurs de programme.
		 *\param[in]	material	Les matériau.
		 */
		C3D_API virtual ProgramFlags getProgramFlags( Material const & material )const noexcept
		{
			return ProgramFlags{};
		}
		/**
		 *\~english
		 *\return		The submesh flags.
		 *\~french
		 *\return		Les indicateurs de submesh.
		 */
		C3D_API virtual SubmeshComponentFlag getSubmeshFlags()const noexcept
		{
			return makeSubmeshComponentFlag( getId() );
		}
		/**
		 *\~english
		 *\return		The buffer usage flags.
		 *\~french
		 *\return		Les flags d'utilisation du buffer.
		 */
		C3D_API virtual VkBufferUsageFlags getUsageFlags()const noexcept
		{
			return {};
		}
		/**
		 *\~english
		 *\return			Clones this component into given submesh.
		 *\param[in,out]	submesh	Receives the cloned component.
		 *\~french
		 *\return			Clone ce composant dans le submesh donné.
		 *\param[in,out]	submesh	Reçoit le composant cloné.
		 */
		C3D_API virtual SubmeshComponentUPtr clone( Submesh & submesh )const = 0;
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		castor::String const & getType()const noexcept
		{
			return m_type;
		}

		void needsUpdate()
		{
			m_dirty = true;
		}

		castor::StringArray const & getDependencies()const noexcept
		{
			return m_dependencies;
		}

		SubmeshComponentPlugin const & getPlugin()const noexcept
		{
			return m_plugin;
		}

		SubmeshComponentID getId()const noexcept
		{
			return m_id;
		}
		/**@}*/

	private:
		C3D_API virtual bool doInitialise( RenderDevice const & device ) = 0;
		C3D_API virtual void doCleanup( RenderDevice const & device ) = 0;
		C3D_API virtual void doUpload( UploadData & uploader ) = 0;

	private:
		castor::String m_type;
		castor::StringArray m_dependencies;
		SubmeshComponentID m_id;
		SubmeshComponentPlugin const& m_plugin;
		bool m_initialised{ false };
		bool m_dirty{ true };
	};
}

#endif
