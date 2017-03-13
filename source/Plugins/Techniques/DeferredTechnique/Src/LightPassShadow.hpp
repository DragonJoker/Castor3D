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
#ifndef ___C3D_LightPassShadow_H___
#define ___C3D_LightPassShadow_H___

#include "DirectionalLightPass.hpp"
#include "PointLightPass.hpp"
#include "SpotLightPass.hpp"

#include <Scene/Light/Light.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslShadow.hpp>

namespace deferred
{
#if !defined( NDEBUG )

	uint32_t g_index = 0u;

#endif
	template< Castor3D::LightType LightType >
	struct LightPassShadowTraits;

	template<>
	struct LightPassShadowTraits< Castor3D::LightType::eDirectional >
	{
		using light_type = Castor3D::DirectionalLight;
		using light_pass_type = DirectionalLightPass;
		using shadow_pass_type = Castor3D::ShadowMapDirectional;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowDirectional;
			return l_name;
		}

		static Castor3D::TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}

		static light_type const & GetTypedLight( Castor3D::Light const & p_light )
		{
			return *p_light.GetDirectionalLight();
		}

		static void DebugDisplay( Castor3D::TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * l_size.width() ), 0 }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}
	};

	template<>
	struct LightPassShadowTraits< Castor3D::LightType::ePoint >
	{
		using light_type = Castor3D::PointLight;
		using light_pass_type = PointLightPass;
		using shadow_pass_type = Castor3D::ShadowMapPoint;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowPoint;
			return l_name;
		}

		static Castor3D::TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture( 0u );
		}

		static light_type const & GetTypedLight( Castor3D::Light const & p_light )
		{
			return *p_light.GetPointLight();
		}

		static void DebugDisplay( Castor3D::TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 128u, 128u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepthCube( Castor::Position{ 0, int32_t( g_index * 3 * l_size.height() ) }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}
	};

	template<>
	struct LightPassShadowTraits< Castor3D::LightType::eSpot >
	{
		using light_type = Castor3D::SpotLight;
		using light_pass_type = SpotLightPass;
		using shadow_pass_type = Castor3D::ShadowMapSpot;

		static Castor::String const & GetName()
		{
			static Castor::String const l_name = GLSL::Shadow::MapShadowSpot;
			return l_name;
		}

		static Castor3D::TextureUnit & GetTexture( shadow_pass_type & p_shadowMap )
		{
			return p_shadowMap.GetTexture();
		}

		static light_type const & GetTypedLight( Castor3D::Light const & p_light )
		{
			return *p_light.GetSpotLight();
		}

		static void DebugDisplay( Castor3D::TextureUnit & p_shadowMap )
		{
			Castor::Size l_size{ 256u, 256u };
			p_shadowMap.GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderDepth( Castor::Position{ int32_t( g_index * l_size.width() ), 0 }
				, l_size
				, *p_shadowMap.GetTexture() );
			++g_index;
		}
	};

	template< Castor3D::LightType LightType >
	class LightPassShadow
		: public LightPassShadowTraits< LightType >::light_pass_type
	{
	public:
		using my_traits = LightPassShadowTraits< LightType >;
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
			Program( Castor3D::Scene const & p_scene
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl )
				: my_program_type{ p_scene, p_vtx, p_pxl }
			{
				my_program_type::m_program->CreateUniform< Castor3D::UniformType::eSampler >( my_traits::GetName()
					, Castor3D::ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
			}
		};

	public:
		LightPassShadow( Castor3D::Engine & p_engine
			, Castor3D::FrameBuffer & p_frameBuffer
			, Castor3D::RenderBufferAttachment & p_depthAttach
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
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera
			, GLSL::FogType p_fogType
			, bool p_first )override
		{
			m_shadowMap.Render( my_traits::GetTypedLight( p_light ) );
			my_pass_type::Update( p_size, p_light, p_camera );
			LightPass::m_frameBuffer.Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
			LightPass::m_depthAttach.Attach( AttachmentPoint::eDepthStencil );
			p_gp[size_t( DsTexture::ePosition )]->Bind();
			p_gp[size_t( DsTexture::eDiffuse )]->Bind();
			p_gp[size_t( DsTexture::eNormals )]->Bind();
			p_gp[size_t( DsTexture::eAmbient )]->Bind();
			p_gp[size_t( DsTexture::eSpecular )]->Bind();
			p_gp[size_t( DsTexture::eEmissive )]->Bind();
			m_shadowMapTexture.Bind();

			auto & l_program = *LightPass::m_programs[uint16_t( p_fogType )];
			l_program.Render( p_size
				, p_light
				, my_pass_type::GetCount()
				, p_first );

			m_shadowMapTexture.Unbind();
			p_gp[size_t( DsTexture::eEmissive )]->Unbind();
			p_gp[size_t( DsTexture::eSpecular )]->Unbind();
			p_gp[size_t( DsTexture::eAmbient )]->Unbind();
			p_gp[size_t( DsTexture::eNormals )]->Unbind();
			p_gp[size_t( DsTexture::eDiffuse )]->Unbind();
			p_gp[size_t( DsTexture::ePosition )]->Unbind();
			LightPass::m_frameBuffer.Unbind();

#if !defined( NDEBUG )

			LightPass::m_frameBuffer.Bind();
			my_traits::DebugDisplay( m_shadowMapTexture );
			LightPass::m_frameBuffer.Unbind();

#endif
		}

	private:
		typename LightPass::ProgramPtr DoCreateProgram( Castor3D::Scene const & p_scene
			, Castor::String const & p_vtx
			, Castor::String const & p_pxl )const override
		{
			return std::make_unique< Program >( p_scene, p_vtx, p_pxl );
		}

	private:
		//!\~english	The shadow map.
		//!\~french		Le mappage d'ombres.
		my_shadow_map_type & m_shadowMap;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		Castor3D::TextureUnit & m_shadowMapTexture;
	};

	using DirectionalLightPassShadow = LightPassShadow< Castor3D::LightType::eDirectional >;
	using PointLightPassShadow = LightPassShadow< Castor3D::LightType::ePoint >;
	using SpotLightPassShadow = LightPassShadow< Castor3D::LightType::eSpot >;
}

#endif
