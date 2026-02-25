#include "Uncharted2ToneMapping/Uncharted2Ubo.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

#include <ShaderWriter/Source.hpp>

namespace Uncharted2
{
	//*********************************************************************************************

	sdw::RetVec3 Uncharted2Data::toneMap( sdw::Vec3 const & px )
	{
		if ( !m_toneMap )
		{
			auto & writer = *getWriter();
			m_toneMap = writer.implementFunction< sdw::Vec3 >( "uncharted2ToneMap"
				, [this, &writer]( sdw::Vec3 const & x )
				{
					writer.returnStmt( (
						(
							x
							* ( x * shoulderStrength + linearAngle * linearStrength )
							+ toeStrength * toeNumerator )
						/ (
							x
							* ( x * shoulderStrength + linearStrength )
							+ toeStrength * toeDenominator ) )
						- toeNumerator / toeDenominator );
				}
				, sdw::InVec3{ writer, "x" } );
		}
		return m_toneMap( px );
	}

	//*********************************************************************************************

	void Uncharted2UboConfiguration::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Uncharted 2" ) );
		visitor.visit( cuT( "Shoulder Strength" ), shoulderStrength );
		visitor.visit( cuT( "Linear Strength" ), linearStrength );
		visitor.visit( cuT( "Linear Angle" ), linearAngle );
		visitor.visit( cuT( "Toe Strength" ), toeStrength );
		visitor.visit( cuT( "Toe Numerator" ), toeNumerator );
		visitor.visit( cuT( "Toe Denominator" ), toeDenominator );
		visitor.visit( cuT( "Linear White Point Value" ), linearWhitePointValue );
		visitor.visit( cuT( "Exposure Bias" ), exposureBias );
	}

	//*********************************************************************************************

	const c3d::String Uncharted2Ubo::Buffer = cuT( "Uncharted2" );
	const c3d::String Uncharted2Ubo::Data = cuT( "Uncharted2Data" );

	Uncharted2Ubo::Uncharted2Ubo( c3d::RenderDevice const & device )
		: UboT{ device }
	{
	}

	void Uncharted2Ubo::update( Configuration const & config )
	{
		setData( config );
	}

	//************************************************************************************************
}
