/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredMeshLightPass_H___
#define ___C3D_DeferredMeshLightPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/Passes/StencilPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"

#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

namespace castor3d
{
	class MeshLightPass
		: public LightPass
	{
	protected:
		/**
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Base class for light passe programs that need a mesh instead of a quad.
		\~french
		\brief		Classe de base pour les programmes de passe d'éclairage nécessitant un maillage plutôt qu'un quad.
		*/
		struct Program
			: public LightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine		The engine.
			 *\param[in]	vtx			The vertex shader source.
			 *\param[in]	pxl			The fragment shader source.
			 *\param[in]	hasShadows	Tells if this program uses shadow map.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine		Le moteur.
			 *\param[in]	vtx			Le source du vertex shader.
			 *\param[in]	pxl			Le source du fagment shader.
			 *\param[in]	hasShadows	Dit si ce programme utilise une shadow map.
			 */
			Program( Engine & engine
				, castor::String const & name
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			/**
			 *\copydoc		castor3d::LightPass::Program::doCreatePipeline
			 */
			ashes::GraphicsPipelinePtr doCreatePipeline( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & renderPass
				, bool blend )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	depthView		The target depth view.
		 *\param[in]	diffuseView		The target diffuse view.
		 *\param[in]	specularView	The target specular view.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	type			The light source type.
		 *\param[in]	hasShadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	depthView		La vue de profondeur cible.
		 *\param[in]	diffuseView		La vue de diffuse cible.
		 *\param[in]	specularView	La vue de spéculaire cible.
		 *\param[in]	gpInfoUbo		L'UBO de la geometry pass.
		 *\param[in]	type			Le type de source lumineuse.
		 *\param[in]	hasShadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		MeshLightPass( Engine & engine
			, castor::String const & suffix
			, ashes::ImageView const & depthView
			, ashes::ImageView const & diffuseView
			, ashes::ImageView const & specularView
			, GpInfoUbo & gpInfoUbo
			, LightType type
			, bool hasShadows );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~MeshLightPass();
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene		The scene.
		 *\param[in]	gp			The geometry pass buffers.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	timer		The render pass timer.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene		La scène.
		 *\param[in]	gp			Les tampons de la geometry pass.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	timer		Le timer de la passe de rendu.
		 */
		void initialise( Scene const & scene
			, GeometryPassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override;
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		void cleanup()override;
		/**
		 *\~english
		 *\brief		Renders the light pass.
		 *\param[in]	index	The lighting pass index.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe de rendu.
		 *\param[in]	index	L'indice de la passe d'éclairage.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente
		 */
		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait )override;
		/**
		 *\~english
		 *\return		The number of primitives to draw.
		 *\~french
		 *\return		Le nombre de primitives à dessiner.
		 */
		uint32_t getCount()const override;

	protected:
		/**
		 *\copydoc		castor3d::LightPass::doUpdate
		 */
		void doUpdate( bool first
			, castor::Size const & size
			, Light const & light
			, Camera const & camera
			, ShadowMap const * shadowMap = nullptr
			, uint32_t shadowMapIndex = 0u )override;
		/**
		 *\copydoc		castor3d::LightPass::doGetVertexShaderSource
		 */
		ShaderPtr doGetVertexShaderSource( SceneFlags const & sceneFlags )const override;
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage.
		 */
		virtual castor::Point3fArray doGenerateVertices()const = 0;
		/**
		 *\~english
		 *\brief		Computes the matrix used to render the model.
		 *\param[in]	light		The light.
		 *\param[in]	camera	The viewing camera.
		 *\return		The matrix.
		 *\~french
		 *\brief		Calcule La matrice utilisée pour dessiner le modèle.
		 *\param[in]	light		La source lumineuse.
		 *\param[in]	camera	La caméra.
		 *\return		La matrice.
		 */
		virtual castor::Matrix4x4f doComputeModelMatrix( Light const & light
			, Camera const & camera )const = 0;

	private:
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		ModelMatrixUbo m_modelMatrixUbo;
		//!\~english	The light's stencil pass.
		//!\~french		La passe stencil de la lumière.
		StencilPass m_stencilPass;
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		LightType m_type;
		//!\~english	The vertex count.
		//!\~french		Le nombre de sommets.
		uint32_t m_count{ 0u };
	};
}

#endif
