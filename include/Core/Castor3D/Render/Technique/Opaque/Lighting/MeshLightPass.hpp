/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshLightPass_H___
#define ___C3D_MeshLightPass_H___

#include "LightingModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/Passes/StencilPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

namespace castor3d
{
	class MeshLightPass
		: public LightPass
	{
	protected:
		/**
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
			 *\param[in]	engine				The engine.
			 *\param[in]	device				The GPU device.
			 *\param[in]	name				The program name.
			 *\param[in]	vtx					The vertex shader source.
			 *\param[in]	pxl					The fragment shader source.
			 *\param[in]	hasShadows			Tells if this program uses shadow map.
			 *\param[in]	hasVoxels			Tells if this program uses voxellisation result.
			 *\param[in]	generatesIndirect	Tells if this program generates indirect lighting.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine				Le moteur.
			 *\param[in]	device				Le device GPU.
			 *\param[in]	name				Le nom du programme.
			 *\param[in]	vtx					Le source du vertex shader.
			 *\param[in]	pxl					Le source du fagment shader.
			 *\param[in]	hasShadows			Dit si ce programme utilise une shadow map.
			 *\param[in]	hasVoxels			Dit si ce programme utilise le résultat de la voxellisation.
			 *\param[in]	generatesIndirect	Dit si ce programme genère de l'éclairage indirect.
			 */
			Program( Engine & engine
				, RenderDevice const & device
				, castor::String const & name
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows
				, bool hasVoxels
				, bool generatesIndirect );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			ashes::GraphicsPipelinePtr doCreatePipeline( ashes::PipelineVertexInputStateCreateInfo const & vertexLayout
				, ashes::RenderPass const & renderPass
				, bool blend )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	suffix			The pass name's suffix.
		 *\param[in]	lpConfig		The light pass configuration.
		 *\param[in]	vctConfig		The voxelizer UBO.
		 *\param[in]	type			The light source type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	suffix			Le suffixe du nom de la passe.
		 *\param[in]	lpConfig		La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig		L'UBO du voxelizer.
		 *\param[in]	type			Le type de source lumineuse.
		 */
		MeshLightPass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, castor::String const & suffix
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig
			, LightType type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~MeshLightPass();
		/**
		 *\copydoc		castor3d::LightPass::initialise
		 */
		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo const & sceneUbo
			, RenderPassTimer & timer )override;
		/**
		 *\copydoc		castor3d::LightPass::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::LightPass::render
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
			, Camera const & camera )override;
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
		UniformBufferT< ModelUboConfiguration > m_modelUbo;
		ModelUboConfiguration * m_modelData;
		StencilPass m_stencilPass;
		LightType m_type;
		uint32_t m_count{ 0u };
	};
}

#endif
