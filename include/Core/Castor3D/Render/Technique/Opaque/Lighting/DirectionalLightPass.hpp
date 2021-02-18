/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredDirectionalLightPass_H___
#define ___C3D_DeferredDirectionalLightPass_H___

#include "LightingModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"

namespace castor3d
{
	class DirectionalLightPass
		: public LightPass
	{
	protected:
		/**
		\~english
		\brief		Directional light pass program.
		\~french
		\brief		Programme de passe de lumière directionnelle.
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
			 *\param[in]	pass				The light pass.
			 *\param[in]	vtx					The vertex shader source.
			 *\param[in]	pxl					The fragment shader source.
			 *\param[in]	hasShadows			Tells if this program uses shadow map.
			 *\param[in]	hasVoxels			Tells if this program uses voxellisation result.
			 *\param[in]	generatesIndirect	Tells if this program generates indirect lighting.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine				Le moteur.
			 *\param[in]	device				Le device GPU.
			 *\param[in]	pass				La passe d'éclairage.
			 *\param[in]	vtx					Le source du vertex shader.
			 *\param[in]	pxl					Le source du fagment shader.
			 *\param[in]	hasShadows			Dit si ce programme utilise une shadow map.
			 *\param[in]	hasVoxels			Dit si ce programme utilise le résultat de la voxellisation.
			 *\param[in]	generatesIndirect	Dit si ce programme genère de l'éclairage indirect.
			 */
			Program( Engine & engine
				, RenderDevice const & device
				, DirectionalLightPass & pass
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows = false
				, bool hasVoxels = false
				, bool generatesIndirect = false );
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
			void doBind( Light const & light )override;

		private:
			DirectionalLightPass & m_lightPass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	suffix			The pass name's suffix.
		 *\param[in]	lpConfig		The light pass configuration.
		 *\param[in]	vctConfig		The voxelizer UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	suffix			Le suffixe du nom de la passe.
		 *\param[in]	lpConfig		La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig		L'UBO du voxelizer.
		 */
		DirectionalLightPass( RenderDevice const & device
			, castor::String const & suffix
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig = nullptr );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	lpConfig		The light pass configuration.
		 *\param[in]	vctConfig		The voxelizer UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	lpConfig		La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig		L'UBO du voxelizer.
		 */
		DirectionalLightPass( RenderDevice const & device
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig = nullptr )
			: DirectionalLightPass{ device
				, castor::String{}
				, lpConfig
				, vctConfig }
		{
		}
		/**
		 *\copydoc		castor3d::LightPass::initialise
		 */
		void initialise( Scene const & scene
			, OpaquePassResult const & gp
			, SceneUbo & sceneUbo
			, RenderPassTimer & timer )override;
		/**
		 *\copydoc		castor3d::LightPass::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::LightPass::accept
		 */
		void accept( PipelineVisitorBase & visitor )override;
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
		 *\copydoc		castor3d::LightPass::doCreateProgram
		 */
		LightPass::ProgramPtr doCreateProgram()override;

	private:
		struct Config
		{
			LightPass::Config base;
			castor::Point4f direction;
			castor::Point4f splitDepths;
			castor::Point4f splitScales;
			std::array< castor::Matrix4x4f, shader::DirectionalMaxCascadesCount > transform;
		};
		UniformBufferUPtrT< Config > m_ubo;
		Viewport m_viewport;
	};
}

#endif
