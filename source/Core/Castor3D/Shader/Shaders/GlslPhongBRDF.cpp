#include "Castor3D/Shader/Shaders/GlslPhongBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	PhongDiffuseBRDF::PhongDiffuseBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	c3d::DiffuseBRDFPtr PhongDiffuseBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, PhongDiffuseBRDF >( writer, brdfHelpers );
	}

	void PhongDiffuseBRDF::doGenerate( c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computePhongDiffuse"
			, [this]( c3d::BlendComponents const & /*components*/
				, c3d::LightSurface const & /*lightSurface*/
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & /*NdotL*/ )
			{
				m_writer.returnStmt( radiance * intensity );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	//*********************************************************************************************

	PhongSpecularBRDF::PhongSpecularBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::SpecularBRDF{ writer, brdfHelpers }
	{
	}

	c3d::SpecularBRDFPtr PhongSpecularBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SpecularBRDF, PhongSpecularBRDF >( writer, brdfHelpers );
	}

	void PhongSpecularBRDF::doGenerate( c3d::BlendComponents const & pcomponents )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computePhongSpecular"
			, [this]( c3d::BlendComponents const & components
				, sdw::Vec3 const & /*N*/
				, sdw::Vec3 const & /*L*/
				, sdw::Vec3 const & /*H*/
				, sdw::Vec3 const & /*V*/
				, sdw::Float const & /*NdotL*/
				, sdw::Float const & NdotH )
			{
				m_writer.returnStmt( pow( NdotH , ( 1.0_f - components.perceptualRoughness ) * 256.0_f ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, sdw::InVec3{ m_writer, "N" }
			, sdw::InVec3{ m_writer, "L" }
			, sdw::InVec3{ m_writer, "H" }
			, sdw::InVec3{ m_writer, "V" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}

	//*********************************************************************************************

	PhongClearcoatBRDF::PhongClearcoatBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::ClearcoatBRDF{ writer, brdfHelpers }
	{
	}

	c3d::ClearcoatBRDFPtr PhongClearcoatBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< ClearcoatBRDF, PhongClearcoatBRDF >( writer, brdfHelpers );
	}

	void PhongClearcoatBRDF::doGenerate( c3d::BlendComponents const & pcomponents )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computePhongClearcoat"
			, [this]( c3d::BlendComponents const & components
				, sdw::Vec3 const & /*N*/
				, sdw::Vec3 const & /*L*/
				, sdw::Vec3 const & /*H*/
				, sdw::Vec3 const & /*V*/
				, sdw::Float const & /*NdotL*/
				, sdw::Float const & NdotH )
			{
				m_writer.returnStmt( pow( NdotH, ( 1.0_f - components.perceptualRoughness ) * 256.0_f ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, sdw::InVec3{ m_writer, "N" }
			, sdw::InVec3{ m_writer, "L" }
			, sdw::InVec3{ m_writer, "H" }
			, sdw::InVec3{ m_writer, "V" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}

	//*********************************************************************************************
}
