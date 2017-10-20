﻿#include "ReinhardToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;
using namespace castor3d;

namespace Reinhard
{
	String ToneMapping::Name = cuT( "reinhard" );

	ToneMapping::ToneMapping( Engine & engine, Parameters const & p_parameters )
		: castor3d::ToneMapping{ Name, engine, p_parameters }
	{
		String param;

		if ( p_parameters.get( cuT( "Gamma" ), param ) )
		{
			m_config.setGamma( string::toFloat( param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( engine, p_parameters );
	}

	glsl::Shader ToneMapping::doCreate()
	{
		m_gammaVar = m_configUbo.createUniform< UniformType::eFloat >( HdrConfigUbo::Gamma );

		glsl::Shader pxl;
		{
			auto writer = getEngine()->getRenderSystem()->createGlslWriter();

			// Shader inputs
			Ubo config{ writer, HdrConfigUbo::BufferHdrConfig, HdrConfigUbo::BindingPoint };
			auto c3d_fExposure = config.declMember< Float >( HdrConfigUbo::Exposure );
			auto c3d_fGamma = config.declMember< Float >( HdrConfigUbo::Gamma );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_rgb = writer.declFragData< Vec4 >( cuT( "pxl_rgb" ), 0 );

			glsl::Utils utils{ writer };
			utils.declareApplyGamma();

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				// Exposure tone mapping
				auto mapped = writer.declLocale( cuT( "mapped" ), vec3( Float( 1.0f ) ) - exp( -hdrColor * c3d_fExposure ) );
				// Gamma correction
				pxl_rgb = vec4( utils.applyGamma( c3d_fGamma, mapped ), 1.0 );
			} );

			pxl = writer.finalise();
		}

		return pxl;
	}

	void ToneMapping::doDestroy()
	{
		m_gammaVar.reset();
	}

	void ToneMapping::doUpdate()
	{
		m_gammaVar->setValue( m_config.getGamma() );
	}

	bool ToneMapping::doWriteInto( TextFile & p_file )
	{
		return p_file.writeText( cuT( " -Gamma=" ) + string::toString( m_config.getGamma() ) ) > 0;
	}
}
