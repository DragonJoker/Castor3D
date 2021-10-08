#include "LightStreaksPostEffect/KawaseUbo.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

using namespace castor;
using namespace castor3d;

namespace light_streaks
{
	//*********************************************************************************************

	KawaseData::KawaseData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, pixelSize{ getMember< sdw::Vec2 >( "pixelSize" ) }
		, direction{ getMember< sdw::Vec2 >( "direction" ) }
		, samples{ getMember< sdw::Int >( "samples" ) }
		, attenuation{ getMember< sdw::Float >( "attenuation" ) }
		, pass{ getMember< sdw::Int >( "pass" ) }
	{
	}

	ast::type::StructPtr KawaseData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_KawaseData" );

		if ( result->empty() )
		{
			result->declMember( "pixelSize", ast::type::Kind::eVec2F );
			result->declMember( "direction", ast::type::Kind::eVec2F );
			result->declMember( "samples", ast::type::Kind::eInt );
			result->declMember( "attenuation", ast::type::Kind::eFloat );
			result->declMember( "pass", ast::type::Kind::eInt );
		}

		return result;
	}

	//*********************************************************************************************

	String const KawaseUbo::Buffer = cuT( "Kawase" );
	String const KawaseUbo::Data = cuT( "c3d_kawaseData" );

	KawaseUbo::KawaseUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
	{
		for ( uint32_t i = 0u; i < PostEffect::Count * 3u; ++i )
		{
			m_ubo.push_back( device.uboPools->getBuffer< Configuration >( 0u ) );
		}
	}

	KawaseUbo::~KawaseUbo()
	{
		for ( auto & ubo : m_ubo )
		{
			m_device.uboPools->putBuffer( ubo );
		}
	}

	void KawaseUbo::update( uint32_t index
		, VkExtent2D const & size
		, castor::Point2f const & direction
		, uint32_t pass )
	{
		Point2f pixelSize{ 1.0f / float( size.width )
			, 1.0f / float( size.height ) };
		auto & data = m_ubo[index].getData();
		data.pixelSize = pixelSize;
		data.direction = direction;
		data.pass = int( pass );
	}

	void KawaseUbo::update( KawaseConfig const & config )
	{
		for ( auto & ubo : m_ubo )
		{
			auto & data = ubo.getData();
			data.samples = config.samples;
			data.attenuation = config.attenuation;
		}
	}

	//************************************************************************************************
}
