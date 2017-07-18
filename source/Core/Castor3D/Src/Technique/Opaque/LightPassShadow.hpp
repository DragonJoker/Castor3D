/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_DeferredLightPassShadow_H___
#define ___C3D_DeferredLightPassShadow_H___

#include "DirectionalLightPass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"
#include "ShadowMap/DeferredShadowMapDirectional.hpp"
#include "ShadowMap/DeferredShadowMapPoint.hpp"
#include "ShadowMap/DeferredShadowMapSpot.hpp"

#include <Engine.hpp>
#include <Scene/Light/Light.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/PushUniform.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslShadow.hpp>

namespace Castor3D
{
#if !defined( NDEBUG )

	uint32_t g_index = 0u;

#endif
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
		using shadow_pass_type = DeferredShadowMapDirectional;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la shadow map.
		 */
		static Castor::String const & GetName()
		{
			static Castor::String const name = GLSL::Shadow::MapShadowDirectional;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	p_shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	p_shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	p_light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	p_light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetDirectionalLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * size.width() ), 0 }
				, size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
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
		using shadow_pass_type = DeferredShadowMapPoint;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la shadow map.
		 */
		static Castor::String const & GetName()
		{
			static Castor::String const name = GLSL::Shadow::MapShadowPoint;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	p_shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	p_shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	p_light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	p_light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetPointLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size size{ 128u, 128u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepthCube( Castor::Position{ 0, int32_t( g_index * 3 * size.height() ) }
				, size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
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
		using shadow_pass_type = DeferredShadowMapSpot;
		/**
		 *\~english
		 *\return		The shadow map name.
		 *\~french
		 *\return		Le nom de la shadow map.
		 */
		static Castor::String const & GetName()
		{
			static Castor::String const name = GLSL::Shadow::MapShadowSpot;
			return name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture from the shadow map.
		 *\param[in]	p_shadowMap	The shadow map.
		 *\return		The shadow map texture.
		 *\~french
		 *\brief		Récupère la texture d'une shadow map.
		 *\param[in]	p_shadowMap	La shadow map.
		 *\return		Le nom de la shadow map.
		 */
		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}
		/**
		 *\~english
		 *\brief		Retrieves the typed light source from a generic one.
		 *\param[in]	p_light	The generic light source.
		 *\return		The typed light source.
		 *\~french
		 *\brief		Récupère la source lumineuse typée depuis une générique.
		 *\param[in]	p_light	La source lumineuse générique.
		 *\return		La source lumineuse typée.
		 */
		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetSpotLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * size.width() ), 0 }
				, size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
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
			 *\param[in]	p_engine	The engine.
			 *\param[in]	p_vtx		The vertex shader source.
			 *\param[in]	p_pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_engine	Le moteur.
			 *\param[in]	p_vtx		Le source du vertex shader.
			 *\param[in]	p_pxl		Le source du fagment shader.
			 */
			Program( Engine & p_engine
				, GLSL::Shader const & p_vtx
				, GLSL::Shader const & p_pxl )
				: my_program_type( p_engine, p_vtx, p_pxl )
			{
				this->m_program->template CreateUniform< UniformType::eSampler >( my_traits::GetName()
					, ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
			}
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\param[in]	p_depthAttach	The depth buffer attach.
		 *\param[in]	p_shadowMap		The shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 *\param[in]	p_depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	p_shadowMap		La map d'ombre.
		 */
		LightPassShadow( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, my_shadow_map_type & p_shadowMap )
			: my_pass_type{ p_engine
				, p_frameBuffer
				, p_depthAttach
				, true }
			, m_shadowMap{ p_shadowMap }
			, m_shadowMapTexture{ my_traits::GetTexture( p_shadowMap ) }
		{
			m_shadowMapTexture.SetIndex( uint32_t( DsTexture::eCount ) );
		}
		/**
		 *\~english
		 *\brief		Renders the light pass on currently bound framebuffer.
		 *\param[in]	p_size			The render area dimensions.
		 *\param[in]	p_gp			The geometry pass result.
		 *\param[in]	p_light			The light.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\param[in]	p_first			Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief		Dessine la passe d'éclairage sur le tampon d'image donné.
		 *\param[in]	p_size			Les dimensions de la zone de rendu.
		 *\param[in]	p_gp			Le résultat de la geometry pass.
		 *\param[in]	p_light			La source lumineuse.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 *\param[in]	p_first			Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Light const & p_light
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool p_first )override
		{
			this->m_gpInfo->Update( p_size
				, p_camera
				, p_invViewProj
				, p_invView
				, p_invProj );

			m_shadowMap.Render( my_traits::GetTypedLight( p_light ) );
			my_pass_type::DoUpdate( p_size
				, p_light
				, p_camera );
			m_shadowMapTexture.Bind();
			this->m_program->Bind( p_light );
			my_pass_type::DoRender( p_size
				, p_gp
				, p_light.GetColour()
				, p_first );

			m_shadowMapTexture.Unbind();

#if 0//!defined( NDEBUG )

			LightPass::m_frameBuffer.Bind( FrameBufferTarget::eDraw );
			my_traits::DebugDisplay( m_shadowMapTexture );
			LightPass::m_frameBuffer.Unbind();

#endif
		}

	private:
		/**
		 *\copydoc		Castor3D::LightPass::DoCreateProgram
		 */
		typename LightPass::ProgramPtr DoCreateProgram( GLSL::Shader const & p_vtx
			, GLSL::Shader const & p_pxl )const override
		{
			return std::make_unique< LightPassShadow::Program >( this->m_engine
				, p_vtx
				, p_pxl );
		}

	private:
		//!\~english	The shadow map.
		//!\~french		Le mappage d'ombres.
		my_shadow_map_type & m_shadowMap;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		TextureUnit & m_shadowMapTexture;
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
