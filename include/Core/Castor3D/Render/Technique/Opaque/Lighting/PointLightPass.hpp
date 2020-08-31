/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PointLightPass_H___
#define ___C3D_PointLightPass_H___

#include "LightingModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/MeshLightPass.hpp"

namespace castor3d
{
	class PointLightPass
		: public MeshLightPass
	{
	protected:
		/**
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Point light pass program.
		\~french
		\brief		Programme de passe de lumière omnidirectionnelle.
		*/
		struct Program
			: public MeshLightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine		The engine.
			 *\param[in]	lightPass	The light pass.
			 *\param[in]	vtx			The vertex shader source.
			 *\param[in]	pxl			The fragment shader source.
			 *\param[in]	hasShadows	Tells if this program uses shadow map.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine		Le moteur.
			 *\param[in]	lightPass	La passe d'éclairage.
			 *\param[in]	vtx			Le source du vertex shader.
			 *\param[in]	pxl			Le source du fagment shader.
			 *\param[in]	hasShadows	Dit si ce programme utilise une shadow map.
			 */
			Program( Engine & engine
				, PointLightPass & lightPass
				, ShaderModule const & vtx
				, ShaderModule const & pxl
				, bool hasShadows = false );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			/**
			 *\copydoc		castor3d::LightPass::Program::doBind
			 */
			void doBind( Light const & light )override;

		private:
			PointLightPass & m_lightPass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	lpResult	The light pass result.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	hasShadows	Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	lpResult	Le résultat de la passe d'éclairage.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	hasShadows	Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		PointLightPass( Engine & engine
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, bool hasShadows = false );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~PointLightPass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
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
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			castor::Point4f position;
			//!\~english	The variable containing the light attenuation (RGB) and index (A).
			//!\~french		La variable contenant l'atténuation de la lumière (RGB) et son index (A).
			castor::Point4f attenuation;
		};
		UniformBufferUPtr< Config > m_ubo;
	};
}

#endif
