#include "FxaaPostEffect/FxaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace fxaa
{
	//*********************************************************************************************

	FxaaData::FxaaData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, pixelSize{ getMember< sdw::Vec2 >( "pixelSize" ) }
		, subpixShift{ getMember< sdw::Float >( "subpixShift" ) }
		, spanMax{ getMember< sdw::Float >( "spanMax" ) }
		, reduceMul{ getMember< sdw::Float >( "reduceMul" ) }
	{
	}

	ast::type::BaseStructPtr FxaaData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_FxaaData" );

		if ( result->empty() )
		{
			result->declMember( "pixelSize", ast::type::Kind::eVec2F );
			result->declMember( "subpixShift", ast::type::Kind::eFloat );
			result->declMember( "spanMax", ast::type::Kind::eFloat );
			result->declMember( "reduceMul", ast::type::Kind::eFloat );
		}

		return result;
	}

	//*********************************************************************************************

	castor::String const FxaaUbo::Buffer = cuT( "Fxaa" );
	castor::String const FxaaUbo::Data = cuT( "c3d_fxaaData" );

	FxaaUbo::FxaaUbo( castor3d::RenderDevice const & device
		, castor::Size const & size )
		: m_device{ device }
		, m_ubo{ device.uboPools->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data.pixelSize = castor::Point2f{ 1.0f / float( size.getWidth() )
			, 1.0f / float( size.getHeight() ) };
	}

	FxaaUbo::~FxaaUbo()
	{
		m_device.uboPools->putBuffer( m_ubo );
	}

	void FxaaUbo::cpuUpdate( float shift
		, float span
		, float reduce )
	{
		auto & data = m_ubo.getData();
		data.subpixShift = shift;
		data.spanMax = span;
		data.reduceMul = reduce;
	}

	//************************************************************************************************
}
