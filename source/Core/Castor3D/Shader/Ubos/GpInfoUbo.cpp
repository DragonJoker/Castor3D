#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		GpInfoData::GpInfoData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_mtxInvViewProj{ getMember< sdw::Mat4 >( "mtxInvViewProj" ) }
			, m_mtxInvView{ getMember< sdw::Mat4 >( "mtxInvView" ) }
			, m_mtxInvProj{ getMember< sdw::Mat4 >( "mtxInvProj" ) }
			, m_mtxGView{ getMember< sdw::Mat4 >( "mtxGView" ) }
			, m_mtxGProj{ getMember< sdw::Mat4 >( "mtxGProj" ) }
			, m_renderSize{ getMember< sdw::Vec2 >( "renderSize" ) }
		{
		}

		GpInfoData & GpInfoData::operator=( GpInfoData const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		ast::type::StructPtr GpInfoData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "GPInfoData" );

			if ( result->empty() )
			{
				result->declMember( "mtxInvViewProj", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxInvView", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxInvProj", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxGView", ast::type::Kind::eMat4x4F );
				result->declMember( "mtxGProj", ast::type::Kind::eMat4x4F );
				result->declMember( "renderSize", ast::type::Kind::eVec2F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > GpInfoData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec3 GpInfoData::readNormal( sdw::Vec3 const & input )const
		{
			return -( transpose( inverse( m_mtxGView ) ) * vec4( input, 1.0 ) ).xyz();
		}

		sdw::Vec3 GpInfoData::writeNormal( sdw::Vec3 const & input )const
		{
			return ( transpose( inverse( m_mtxInvView ) ) * vec4( -input, 1.0 ) ).xyz();
		}

		sdw::Vec3 GpInfoData::projToWorld( Utils const & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, m_mtxInvViewProj );
		}

		sdw::Vec3 GpInfoData::projToView( Utils const & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcVSPosition( texCoord, depth, m_mtxInvProj );
		}

		sdw::Vec2 GpInfoData::calcTexCoord( Utils const & utils
			, sdw::Vec2 const & fragCoord )const
		{
			return utils.calcTexCoord( fragCoord, m_renderSize );
		}
	}

	//*********************************************************************************************

	const castor::String GpInfoUbo::BufferGPInfo = cuT( "GPInfo" );
	const castor::String GpInfoUbo::GPInfoData = cuT( "GPInfoData" );

	GpInfoUbo::GpInfoUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise( engine.getRenderSystem()->getCurrentRenderDevice() );
		}
	}

	GpInfoUbo::~GpInfoUbo()
	{
	}

	void GpInfoUbo::initialise( castor3d::RenderDevice const & device )
	{
		if ( !m_ubo )
		{
			m_ubo = device.uboPools->getBuffer< GpInfoUboConfiguration >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
		}
	}

	void GpInfoUbo::cleanup( castor3d::RenderDevice const & device )
	{
		if ( m_ubo )
		{
			device.uboPools->putBuffer< GpInfoUboConfiguration >( m_ubo );
		}
	}

	void GpInfoUbo::cpuUpdate( castor::Size const & renderSize
		, Camera const & camera )
	{
		CU_Require( m_ubo );

		auto invView = camera.getView().getInverse();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();

		auto & configuration = m_ubo.getData();
		configuration.invViewProj = invViewProj;
		configuration.invView = invView;
		configuration.invProj = invProj;
		configuration.gView = camera.getView();
		configuration.gProj = camera.getProjection();
		configuration.renderSize = castor::Point2f( renderSize.getWidth(), renderSize.getHeight() );
	}

	//************************************************************************************************
}
