#include "Uncharted2ToneMapping/Uncharted2Ubo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace Uncharted2
{
	//*********************************************************************************************

	Uncharted2Data::Uncharted2Data( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, shoulderStrength{ getMember< sdw::Float >( "shoulderStrength" ) }
		, linearStrength{ getMember< sdw::Float >( "linearStrength" ) }
		, linearAngle{ getMember< sdw::Float >( "linearAngle" ) }
		, toeStrength{ getMember< sdw::Float >( "toeStrength" ) }
		, toeNumerator{ getMember< sdw::Float >( "toeNumerator" ) }
		, toeDenominator{ getMember< sdw::Float >( "toeDenominator" ) }
		, linearWhitePointValue{ getMember< sdw::Float >( "linearWhitePointValue" ) }
		, exposureBias{ getMember< sdw::Float >( "exposureBias" ) }
	{
	}

	ast::type::BaseStructPtr Uncharted2Data::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_Uncharted2Data" );

		if ( result->empty() )
		{
			result->declMember( "shoulderStrength", ast::type::Kind::eFloat );
			result->declMember( "linearStrength", ast::type::Kind::eFloat );
			result->declMember( "linearAngle", ast::type::Kind::eFloat );
			result->declMember( "toeStrength", ast::type::Kind::eFloat );
			result->declMember( "toeNumerator", ast::type::Kind::eFloat );
			result->declMember( "toeDenominator", ast::type::Kind::eFloat );
			result->declMember( "linearWhitePointValue", ast::type::Kind::eFloat );
			result->declMember( "exposureBias", ast::type::Kind::eFloat );
		}

		return result;
	}

	std::unique_ptr< sdw::Struct > Uncharted2Data::declare( sdw::ShaderWriter & writer )
	{
		return std::make_unique< sdw::Struct >( writer
			, makeType( writer.getTypesCache() ) );
	}

	//*********************************************************************************************

	const castor::String Uncharted2Ubo::Buffer = cuT( "Uncharted2" );
	const castor::String Uncharted2Ubo::Data = cuT( "Uncharted2Data" );

	Uncharted2Ubo::Uncharted2Ubo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data = Configuration{};
	}

	Uncharted2Ubo::~Uncharted2Ubo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	//************************************************************************************************
}
