/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKYBOX_H___
#define ___C3D_SKYBOX_H___

#include "Render/Viewport.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "PBR/IblTextures.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		Skybox implementation.
	\~french
	\brief		Implémentation de Skybox.
	*/
	class Skybox
		: public castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Skybox loader.
		\~english
		\brief		Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< Skybox >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a Skybox into a text file.
			 *\param[in]	obj		The Skybox to save.
			 *\param[in]	file	The file to write the Skybox in.
			 *\~french
			 *\brief		Ecrit une Skybox dans un fichier texte.
			 *\param[in]	obj		La Skybox.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( Skybox const & obj, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit Skybox( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Skybox();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void cleanup();
		/**
		 *\~english
		 *\brief		Updates the skybox.
		 *\param[in]	camera	The scene's camera.
		 *\~french
		 *\brief		Dessine la skybox.
		 *\param[in]	camera	La caméra de la scène.
		 */
		C3D_API void update( Camera const & camera );
		/**
		*\~english
		*\return		Sets the skybox's equirectangular texture.
		*\~french
		*\return		Définit la texture équirectangulaire de la skybox.
		*/
		C3D_API void setEquiTexture( TextureLayoutSPtr texture
			, castor::Size const & size );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::Path const & getEquiTexturePath()const
		{
			return m_equiTexturePath;
		}

		inline TextureLayout const & getTexture()const
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline TextureLayout & getTexture()
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline renderer::Texture const & getImage()const
		{
			REQUIRE( m_texture );
			return m_texture->getTexture();
		}

		inline renderer::TextureView const & getView()const
		{
			REQUIRE( m_texture );
			return m_texture->getDefaultView();
		}

		inline IblTextures const & getIbl()const
		{
			REQUIRE( m_ibl );
			return *m_ibl;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_semaphore );
			return *m_semaphore;
		}

		inline renderer::CommandBuffer const & getCommandBuffer()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setScene( Scene & scene )
		{
			m_scene = &scene;
		}

		inline void setTexture( TextureLayoutSPtr texture )
		{
			m_texture = std::move( texture );
		}
		/**@}*/

	protected:
		virtual renderer::ShaderStageStateArray doInitialiseShader();
		bool doInitialiseTexture();
		void doInitialiseEquiTexture();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( renderer::ShaderStageStateArray & program );
		bool doPrepareFrame();

	protected:
		SceneRPtr m_scene{ nullptr };
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::DescriptorSetLayoutPtr m_descriptorSetLayout;
		renderer::DescriptorSetPoolPtr m_descriptorSetPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_semaphore;
		TextureLayoutSPtr m_equiTexture;
		castor::Path m_equiTexturePath;
		castor::Size m_equiSize;
		TextureLayoutSPtr m_texture;
		SamplerWPtr m_sampler;
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		HdrConfigUbo m_configUbo;
		renderer::VertexBufferPtr< NonTexturedCube > m_vertexBuffer{ nullptr };
		std::vector< NonTexturedCube > m_bufferVertex;
		renderer::VertexLayoutPtr m_declaration;
		castor::Matrix4x4r m_mtxModel;
		std::unique_ptr< IblTextures > m_ibl;
		bool m_hdr{ false };
		Viewport m_viewport;
		castor::Size m_size;
	};
}

#endif
