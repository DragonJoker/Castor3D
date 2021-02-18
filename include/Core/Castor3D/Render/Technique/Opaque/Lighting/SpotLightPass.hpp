/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpotLightPass_H___
#define ___C3D_SpotLightPass_H___

#include "LightingModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/MeshLightPass.hpp"

namespace castor3d
{
	class SpotLightPass
		: public MeshLightPass
	{
	protected:
		/**
		\~english
		\brief		Spot light pass program.
		\~french
		\brief		Programme de passe de lumière projecteur.
		*/
		struct Program
			: public MeshLightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine				The engine.
			 *\param[in]	device				The GPU device.
			 *\param[in]	lightPass			The light pass.
			 *\param[in]	vtx					The vertex shader source.
			 *\param[in]	pxl					The fragment shader source.
			 *\param[in]	hasShadows			Tells if this program uses shadow map.
			 *\param[in]	hasVoxels			Tells if this program uses voxellisation result.
			 *\param[in]	generatesIndirect	Tells if this program generates indirect lighting.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine				Le moteur.
			 *\param[in]	device				Le device GPU.
			 *\param[in]	lightPass			La passe d'éclairage.
			 *\param[in]	vtx					Le source du vertex shader.
			 *\param[in]	pxl					Le source du fagment shader.
			 *\param[in]	hasShadows			Dit si ce programme utilise une shadow map.
			 *\param[in]	hasVoxels			Dit si ce programme utilise le résultat de la voxellisation.
			 *\param[in]	generatesIndirect	Dit si ce programme genère de l'éclairage indirect.
			 */
			Program( Engine & engine
				, RenderDevice const & device
				, SpotLightPass & lightPass
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
			void doBind( Light const & light )override;

		private:
			SpotLightPass & m_lightPass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	suffix		The pass name's suffix.
		 *\param[in]	lpConfig	The light pass configuration.
		 *\param[in]	vctConfig	The voxelizer UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	suffix		Le suffixe du nom de la passe.
		 *\param[in]	lpConfig	La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig	L'UBO du voxelizer.
		 */
		SpotLightPass( RenderDevice const & device
			, castor::String const & suffix
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig = nullptr );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	lpConfig	The light pass configuration.
		 *\param[in]	vctConfig	The voxelizer UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	lpConfig	La configuration de la passe d'éclairage.
		 *\param[in]	vctConfig	L'UBO du voxelizer.
		 */
		SpotLightPass( RenderDevice const & device
			, LightPassConfig const & lpConfig
			, VoxelizerUbo const * vctConfig = nullptr )
			: SpotLightPass{ device
				, castor::String{}
				, lpConfig
				, vctConfig }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SpotLightPass();
		/**
		 *\copydoc		castor3d::LightPass::accept
		 */
		void accept( PipelineVisitorBase & visitor )override;

	private:
		/**
		 *\copydoc		castor3d::LightPass::doCreateProgram
		 */
		LightPass::ProgramPtr doCreateProgram()override;
		/**
		 *\copydoc		castor3d::MeshLightPass::doGenerateVertices
		 */
		castor::Point3fArray doGenerateVertices()const override;
		/**
		 *\copydoc		castor3d::MeshLightPass::doComputeModelMatrix
		 */
		castor::Matrix4x4f doComputeModelMatrix( Light const & light
			, Camera const & camera )const override;

	private:
		struct Config
		{
			LightPass::Config base;
			//!\~english	The variable containing the light position (RGB).
			//!\~french		La variable contenant la position de la lumière (RGB).
			castor::Point4f position;
			//!\~english	The variable containing the light attenuation (RGB) and index (A).
			//!\~french		La variable contenant l'atténuation de la lumière (RGB) et son index (A).
			castor::Point4f attenuation;
			//!\~english	The variable containing the light direction (RGB).
			//!\~french		La variable contenant la direction de la lumière (RGB).
			castor::Point4f direction;
			//!\~english	The variable containing the light exponent (R) and cutoff (G).
			//!\~french		La variable contenant l'exposant de la lumière (R) et l'angle de son cône (G).
			castor::Point4f exponentCutOff;
			//!\~english	The variable containing the light space transformation matrix.
			//!\~french		La variable contenant la matrice de transformation de la lumière.
			castor::Matrix4x4f transform;
		};
		UniformBufferUPtrT< Config > m_ubo;
	};
}

#endif
