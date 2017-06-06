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
#include "ShadowMapDirectional.hpp"
#include "ShadowMapPoint.hpp"
#include "ShadowMapSpot.hpp"

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
	template< LightType LtType >
	struct LightPassShadowTraits;

	template<>
	struct LightPassShadowTraits< LightType::eDirectional >
	{
		using light_type = DirectionalLight;
		using light_pass_type = DirectionalLightPass;
		using shadow_pass_type = DeferredShadowMapDirectional;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowDirectional;
			return l_name;
		}

		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}

		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetDirectionalLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * l_size.width() ), 0 }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
	};

	template<>
	struct LightPassShadowTraits< LightType::ePoint >
	{
		using light_type = PointLight;
		using light_pass_type = PointLightPass;
		using shadow_pass_type = DeferredShadowMapPoint;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowPoint;
			return l_name;
		}

		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}

		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetPointLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 128u, 128u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepthCube( Castor::Position{ 0, int32_t( g_index * 3 * l_size.height() ) }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
	};

	template<>
	struct LightPassShadowTraits< LightType::eSpot >
	{
		using light_type = SpotLight;
		using light_pass_type = SpotLightPass;
		using shadow_pass_type = DeferredShadowMapSpot;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowSpot;
			return l_name;
		}

		static TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}

		static light_type const & GetTypedLight( Light const & p_light )
		{
			return *p_light.GetSpotLight();
		}

#if !defined( NDEBUG )

		static void DebugDisplay( TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * l_size.width() ), 0 }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}

#endif
	};

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
		struct Program
			: public my_pass_type::Program
		{
		public:
			using my_program_type = typename my_pass_type::Program;

		public:
			Program( Engine & p_engine
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl )
				: my_program_type( p_engine, p_vtx, p_pxl )
			{
				this->m_program->template CreateUniform< UniformType::eSampler >( my_traits::GetName()
					, ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
			}
		};

	public:
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
		typename LightPass::ProgramPtr DoCreateProgram( Castor::String const & p_vtx
			, Castor::String const & p_pxl )const override
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

	using DirectionalLightPassShadow = LightPassShadow< LightType::eDirectional >;
	using PointLightPassShadow = LightPassShadow< LightType::ePoint >;
	using SpotLightPassShadow = LightPassShadow< LightType::eSpot >;
}

#endif
