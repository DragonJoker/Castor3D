#include "FxaaPostEffect/FxaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

namespace fxaa
{
	//*********************************************************************************************

	FxaaData::FxaaData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, c3d::move( expr ), enabled }
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

	c3d::MbString const FxaaUbo::Buffer = "Fxaa";
	c3d::MbString const FxaaUbo::Data = "c3d_fxaaData";

	FxaaUbo::FxaaUbo( c3d::RenderDevice const & device
		, c3d::Size const & size )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
		auto & data = m_ubo.getData();
		data.pixelSize = c3d::Point2f{ 1.0f / float( size.getWidth() )
			, 1.0f / float( size.getHeight() ) };
	}

	FxaaUbo::~FxaaUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
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
