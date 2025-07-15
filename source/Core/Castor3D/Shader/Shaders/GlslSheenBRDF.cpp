#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementDeleter( c3d::shader, SheenBRDF )

namespace c3d::shader
{
	SheenBRDF::SheenBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{
	}

	sdw::RetVec4 SheenBRDF::compute( Utils & utils
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & NdotL
		, sdw::Float const & NdotH )
	{
		if ( !m_compute )
		{
			doGenerate( utils, components, lightSurface );
		}

		return m_compute( components
			, lightSurface
			, NdotL
			, NdotH );
	}

	SheenBRDFPtr SheenBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return makeUnique< SheenBRDF >( writer, brdfHelpers );
	}

	void SheenBRDF::doGenerate( Utils & utils
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeSheen"
			, [this, &utils]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto NdotV = lightSurface.NdotV().value();
				auto const & sheenRoughness = components.sheenRoughness;
				auto sheenDistribution = m_writer.declLocale( "sheenDistribution"
					, m_brdfHelpers.distributionCharlie( NdotH, sheenRoughness ) );
				auto sheenVisibility = m_writer.declLocale( "sheenVisibility"
					, m_brdfHelpers.visibilitySheen( NdotV, NdotL, sheenRoughness ) );
				m_writer.returnStmt( vec4( components.sheenColour * sheenDistribution * sheenVisibility
					, max( utils.directionalAlbedoSheen( NdotV, sheenRoughness )
						, utils.directionalAlbedoSheen( NdotL, sheenRoughness ) ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}
}
