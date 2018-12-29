/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPassShadow_H___
#define ___C3D_DeferredLightPassShadow_H___

#include "DirectionalLightPass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"
#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"

#include "Engine.hpp"
#include "Scene/Light/Light.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/Sampler.hpp"

#include "Shader/Shaders/GlslShadow.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Traits structure to specialise light passes with shadows.
	\~french
	\brief		Structure de traits pour spécialiser les passes d'éclairage avec des ombres.
	*/
	template< LightType LtType >
	struct LightPassShadowTraits;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Traits structure to specialise light passes with shadows.
	\remarks	Specialisation for LightType::eDirectional.
	\~french
	\brief		Structure de traits pour spécialiser les passes d'éclairage avec des ombres.
	\remarks	Spécialisation pour LightType::eDirectional.
	*/
	template<>
	struct LightPassShadowTraits< LightType::eDirectional >
	{
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		using light_type = DirectionalLight;
		//!\~english	The light pass type.
		//!\~french		Le type de passe d'éclairage.
		using light_pass_type = DirectionalLightPass;
		//!\~english	The shadow map type.
		//!\~french		Le type de map d'ombre.
		using shadow_pass_type = ShadowMapDirectional;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la texture d'ombres.
		 */
		static castor::String const & getShadowMapName()
		{
			static castor::String const name = shader::Shadow::MapShadowDirectional;
			return name;
		}
		/**
		 *\~english
		 *\return		The depth map name.
		 *\~french
		 *\return		Le nom de la texture de profondeur.
		 */
		static castor::String const & getDepthMapName()
		{
			static castor::String const name = shader::Shadow::MapDepthDirectional;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & getTexture( shadow_pass_type & shadowMap )
		{
			return shadowMap.getTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & getTypedLight( Light const & light )
		{
			return *light.getDirectionalLight();
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Traits structure to specialise light passes with shadows.
	\remarks	Specialisation for LightType::ePoint.
	\~french
	\brief		Structure de traits pour spécialiser les passes d'éclairage avec des ombres.
	\remarks	Spécialisation pour LightType::ePoint.
	*/
	template<>
	struct LightPassShadowTraits< LightType::ePoint >
	{
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		using light_type = PointLight;
		//!\~english	The light pass type.
		//!\~french		Le type de passe d'éclairage.
		using light_pass_type = PointLightPass;
		//!\~english	The shadow map type.
		//!\~french		Le type de map d'ombre.
		using shadow_pass_type = ShadowMapPoint;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la texture d'ombres.
		 */
		static castor::String const & getShadowMapName()
		{
			static castor::String const name = shader::Shadow::MapShadowPoint;
			return name;
		}
		/**
		 *\~english
		 *\return		The depth map name.
		 *\~french
		 *\return		Le nom de la texture de profondeur.
		 */
		static castor::String const & getDepthMapName()
		{
			static castor::String const name = shader::Shadow::MapDepthPoint;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & getTexture( shadow_pass_type & shadowMap )
		{
			return shadowMap.getTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & getTypedLight( Light const & light )
		{
			return *light.getPointLight();
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Traits structure to specialise light passes with shadows.
	\remarks	Specialisation for LightType::eSpot.
	\~french
	\brief		Structure de traits pour spécialiser les passes d'éclairage avec des ombres.
	\remarks	Spécialisation pour LightType::eSpot.
	*/
	template<>
	struct LightPassShadowTraits< LightType::eSpot >
	{
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		using light_type = SpotLight;
		//!\~english	The light pass type.
		//!\~french		Le type de passe d'éclairage.
		using light_pass_type = SpotLightPass;
		//!\~english	The shadow map type.
		//!\~french		Le type de map d'ombre.
		using shadow_pass_type = ShadowMapSpot;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la texture d'ombres.
		 */
		static castor::String const & getShadowMapName()
		{
			static castor::String const name = shader::Shadow::MapShadowSpot;
			return name;
		}
		/**
		 *\~english
		 *\return		The depth map name.
		 *\~french
		 *\return		Le nom de la texture de profondeur.
		 */
		static castor::String const & getDepthMapName()
		{
			static castor::String const name = shader::Shadow::MapDepthSpot;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & getTexture( shadow_pass_type & shadowMap )
		{
			return shadowMap.getTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & getTypedLight( Light const & light )
		{
			return *light.getSpotLight();
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Base class for all light passes with shadow.
	\~french
	\brief		Classe de base pour toutes les passes d'éclairage avec des ombres.
	*/
	template< LightType LtType >
	class LightPassShadow
		: public LightPassShadowTraits< LtType >::light_pass_type
	{
	public:
		using my_traits = LightPassShadowTraits< LtType >;
		using my_light_type = typename my_traits::light_type;
		using my_pass_type = typename my_traits::light_pass_type;
		using my_shadow_map_type = typename my_traits::shadow_pass_type;

	private:
		/*!
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Base class for all light pass programs with shadow.
		\~french
		\brief		Classe de base pour tous les programme des passes d'éclairage avec des ombres.
		*/
		struct Program
			: public my_pass_type::Program
		{
		public:
			using my_program_type = typename my_pass_type::Program;

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
				, LightPassShadow & lightPass
				, ShaderModule const & vtx
				, ShaderModule const & pxl )
				: my_program_type( engine, lightPass, vtx, pxl, true )
			{
			}
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	frameBuffer	The target framebuffer.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 */
		LightPassShadow( Engine & engine
			, ashes::TextureView const & depthView
			, ashes::TextureView const & diffuseView
			, ashes::TextureView const & specularView
			, GpInfoUbo & gpInfoUbo )
			: my_pass_type{ engine
				, depthView
				, diffuseView
				, specularView
				, gpInfoUbo
				, true }
		{
		}
		/**
		 *\copydoc		castor3d::LightPass::render
		 */
		ashes::Semaphore const & render( uint32_t index
			, ashes::Semaphore const & toWait
			, ShadowMap const * shadowMap
			, uint32_t shadowMapIndex )override
		{
			this->doPrepareCommandBuffer( *this->m_pipeline, shadowMap, shadowMapIndex, !index );
			return my_pass_type::render( index, toWait, nullptr, 0u );
		}

	private:
		/**
		 *\copydoc		castor3d::LightPass::doCreateProgram
		 */
		typename LightPass::ProgramPtr doCreateProgram()override
		{
			auto result = std::make_unique< LightPassShadow::Program >( this->m_engine
				, *this
				, this->m_vertexShader
				, this->m_pixelShader );
			return result;
		}
	};
	//!\~english	The directional lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières directionnelles.
	using DirectionalLightPassShadow = LightPassShadow< LightType::eDirectional >;
	//!\~english	The point lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières omnidirectionnelles.
	using PointLightPassShadow = LightPassShadow< LightType::ePoint >;
	//!\~english	The spot lights light pass with shadows.
	//!\~french		La passe d'éclairage avec ombres pour les lumières projecteurs.
	using SpotLightPassShadow = LightPassShadow< LightType::eSpot >;
}

#endif
