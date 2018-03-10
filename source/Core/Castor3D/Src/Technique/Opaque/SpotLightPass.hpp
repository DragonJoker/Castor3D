/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredSpotLightPass_H___
#define ___C3D_DeferredSpotLightPass_H___

#include "MeshLightPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Spot light pass.
	\~french
	\brief		Passe de lumière projecteur.
	*/
	class SpotLightPass
		: public MeshLightPass
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
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
			 *\param[in]	engine	The engine.
			 *\param[in]	vtx		The vertex shader source.
			 *\param[in]	pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine	Le moteur.
			 *\param[in]	vtx		Le source du vertex shader.
			 *\param[in]	pxl		Le source du fagment shader.
			 */
			Program( Engine & engine
				, glsl::Shader const & vtx
				, glsl::Shader const & pxl );
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
			struct Config
			{
				LightPass::Program::Config m_base;
				//!\~english	The variable containing the light position.
				//!\~french		La variable contenant la position de la lumière.
				castor::Point3f position;
				//!\~english	The variable containing the light attenuation.
				//!\~french		La variable contenant l'atténuation de la lumière.
				castor::Point3f attenuation;
				//!\~english	The variable containing the light direction.
				//!\~french		La variable contenant la direction de la lumière.
				castor::Point3f direction;
				//!\~english	The variable containing the light exponent.
				//!\~french		La variable contenant l'exposant de la lumière.
				float exponent;
				//!\~english	The variable containing the light cut off.
				//!\~french		La variable contenant l'angle du cône de la lumière.
				float cutOff;
				//!\~english	The variable containing the light space transformation matrix.
				//!\~french		La variable contenant la matrice de transformation de la lumière.
				renderer::Mat4 transform;
			};
			renderer::UniformBufferPtr< Config > m_ubo;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	frameBuffer	The target framebuffer.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	hasShadows	Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	hasShadows	Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		SpotLightPass( Engine & engine
			, FrameBuffer & frameBuffer
			, FrameBufferAttachment & depthAttach
			, GpInfoUbo & gpInfoUbo
			, bool hasShadows );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~SpotLightPass();

	private:
		/**
		 *\copydoc		castor3d::LightPass::doCreateProgram
		 */
		LightPass::ProgramPtr doCreateProgram( glsl::Shader const & vtx
			, glsl::Shader const & pxl )const override;
		/**
		 *\copydoc		castor3d::MeshLightPass::doGenerateVertices
		 */
		castor::Point3fArray doGenerateVertices()const override;
		/**
		 *\copydoc		castor3d::MeshLightPass::doComputeModelMatrix
		 */
		castor::Matrix4x4r doComputeModelMatrix( Light const & light
			, Camera const & camera )const override;
	};
}

#endif
