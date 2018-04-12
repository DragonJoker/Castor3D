/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkyboxBackground_H___
#define ___C3D_SkyboxBackground_H___

#include "Scene/Background/Background.hpp"
#include "Render/Viewport.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "PBR/IblTextures.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/**
	*\version
	*	0.9.0
	*\~english
	*\brief
	*	Skybox implementation.
	*\~french
	*\brief
	*	Implémentation de Skybox.
	*/
	class SkyboxBackground
		: public SceneBackground
	{
	public:
		/**
		*\~english
		*\brief
		*	Skybox loader.
		*\~english
		*\brief
		*	Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< SkyboxBackground >
		{
		public:
			/**
			*\~english
			*\brief
			*	Constructor.
			*\~french
			*\brief
			*	Constructeur.
			*/
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			*\~english
			*\brief
			*	Writes a Skybox into a text file.
			*\param[in] obj
			*	The Skybox to save.
			*\param[in] file
			*	The file to write the Skybox in.
			*\~french
			*\brief
			*	Ecrit une Skybox dans un fichier texte.
			*\param[in] obj
			*	La Skybox.
			*\param[in] file
			*	Le fichier.
			*/
			C3D_API bool operator()( SkyboxBackground const & obj, castor::TextFile & file )override;
		};

	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] engine
		*	The engine.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] engine
		*	Le moteur.
		*/
		C3D_API explicit SkyboxBackground( Engine & engine
			, Scene & scene );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~SkyboxBackground();
		/**
		*\~english
		*\return
		*	Sets the skybox's equirectangular texture.
		*\~french
		*\return
		*	Définit la texture équirectangulaire de la skybox.
		*/
		C3D_API void setEquiTexture( TextureLayoutSPtr texture
			, castor::Size const & size );
		/**
		*\copydoc	castor3d::SceneBackground::accept
		*/
		C3D_API void accept( BackgroundVisitor & visitor )const override;
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
		inline void setTexture( TextureLayoutSPtr texture )
		{
			m_texture = std::move( texture );
		}
		/**@}*/

	private:
		/**
		*\copydoc	castor3d::SceneBackground::doInitialise
		*/
		bool doInitialise( renderer::RenderPass const & renderPass )override;
		/**
		*\copydoc	castor3d::SceneBackground::doCleanup
		*/
		void doCleanup()override;
		/**
		*\copydoc	castor3d::SceneBackground::doUpdate
		*/
		void doUpdate( Camera const & camera );

	private:
		virtual renderer::ShaderStageStateArray doInitialiseShader();
		bool doInitialiseTexture();
		void doInitialiseEquiTexture();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( renderer::ShaderStageStateArray & program
			, renderer::RenderPass const & renderPass );

	private:
		TextureLayoutSPtr m_equiTexture;
		castor::Path m_equiTexturePath;
		castor::Size m_equiSize;
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Matrix4x4r m_mtxModel;
		Viewport m_viewport;
	};
}

#endif
