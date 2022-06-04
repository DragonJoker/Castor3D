#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/Frustum.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		MatrixData::MatrixData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_projection{ getMember< sdw::Mat4 >( "projection" ) }
			, m_invProjection{ getMember< sdw::Mat4 >( "invProjection" ) }
			, m_curView{ getMember< sdw::Mat4 >( "curView" ) }
			, m_prvView{ getMember< sdw::Mat4 >( "prvView" ) }
			, m_curViewProj{ getMember< sdw::Mat4 >( "curViewProj" ) }
			, m_prvViewProj{ getMember< sdw::Mat4 >( "prvViewProj" ) }
			, m_jitter{ getMember< sdw::Vec4 >( "jitter" ) }
			, m_frustumPlanes{ getMemberArray< sdw::Vec4 >( "frustumPlanes" ) }
		{
		}

		ast::type::BaseStructPtr MatrixData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MatrixData" );

			if ( result->empty() )
			{
				result->declMember( "projection", ast::type::Kind::eMat4x4F );
				result->declMember( "invProjection", ast::type::Kind::eMat4x4F );
				result->declMember( "curView", ast::type::Kind::eMat4x4F );
				result->declMember( "prvView", ast::type::Kind::eMat4x4F );
				result->declMember( "curViewProj", ast::type::Kind::eMat4x4F );
				result->declMember( "prvViewProj", ast::type::Kind::eMat4x4F );
				result->declMember( "jitter", ast::type::Kind::eVec4F );
				result->declMember( "frustumPlanes", ast::type::Kind::eVec4F, 6u );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > MatrixData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec4 MatrixData::projToView( sdw::Vec4 const & psPosition )const
		{
			return m_invProjection * psPosition;
		}

		sdw::Vec4 MatrixData::viewToProj( sdw::Vec4 const & vsPosition )const
		{
			return m_projection * vsPosition;
		}

		sdw::Vec4 MatrixData::worldToCurView( sdw::Vec4 const & wsPosition )const
		{
			return m_curView * wsPosition;
		}

		sdw::Vec4 MatrixData::worldToPrvView( sdw::Vec4 const & wsPosition )const
		{
			return m_prvView * wsPosition;
		}

		sdw::Vec4 MatrixData::curViewToWorld( sdw::Vec4 const & vsPosition )const
		{
			return inverse( m_curView ) * vsPosition;
		}

		sdw::Vec4 MatrixData::prvViewToWorld( sdw::Vec4 const & vsPosition )const
		{
			return inverse( m_prvView ) * vsPosition;
		}

		sdw::Vec4 MatrixData::worldToCurProj( sdw::Vec4 const & wsPosition )const
		{
			return m_curViewProj * wsPosition;
		}

		sdw::Vec4 MatrixData::worldToPrvProj( sdw::Vec4 const & wsPosition )const
		{
			return m_prvViewProj * wsPosition;
		}

		sdw::Vec4 MatrixData::viewToScreenUV( Utils & utils
			, sdw::Vec4 vsPosition )const
		{
			return utils.clipToScreen( viewToProj( vsPosition ) );
		}

		sdw::Vec4 MatrixData::worldToCurScreenUV( Utils & utils
			, sdw::Vec4 wsPosition )const
		{
			return utils.clipToScreen( worldToCurProj( wsPosition ) );
		}

		sdw::Vec4 MatrixData::worldToPrvScreenUV( Utils & utils
			, sdw::Vec4 wsPosition )const
		{
			return utils.clipToScreen( worldToPrvProj( wsPosition ) );
		}

		sdw::Vec3 MatrixData::projToView( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, m_invProjection );
		}

		sdw::Vec4 MatrixData::curProjToWorld( sdw::Vec4 const & position )const
		{
			return inverse( m_curViewProj ) * position;
		}

		sdw::Vec3 MatrixData::curProjToWorld( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, inverse( m_curViewProj ) );
		}

		sdw::Vec3 MatrixData::prvProjToWorld( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, inverse( m_prvViewProj ) );
		}

		sdw::Vec3 MatrixData::getCurViewRight()const
		{
			return vec3( m_curView[0][0], m_curView[1][0], m_curView[2][0] );
		}

		sdw::Vec3 MatrixData::getPrvViewRight()const
		{
			return vec3( m_prvView[0][0], m_prvView[1][0], m_prvView[2][0] );
		}

		sdw::Vec3 MatrixData::getCurViewUp()const
		{
			return vec3( m_curView[0][1], m_curView[1][1], m_curView[2][1] );
		}

		sdw::Vec3 MatrixData::getPrvViewUp()const
		{
			return vec3( m_prvView[0][1], m_prvView[1][1], m_prvView[2][1] );
		}

		sdw::Vec3 MatrixData::getCurViewCenter()const
		{
			return m_curView[3].xyz();
		}

		sdw::Vec3 MatrixData::getPrvViewCenter()const
		{
			return m_prvView[3].xyz();
		}

		void MatrixData::jitter( sdw::Vec4 & csPosition )const
		{
			csPosition.xy() -= m_jitter.xy() * csPosition.w();
		}
	}

	//*********************************************************************************************

	castor::String const MatrixUbo::BufferMatrix = cuT( "Matrices" );
	castor::String const MatrixUbo::MatrixData = cuT( "c3d_matrixData" );

	MatrixUbo::MatrixUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	MatrixUbo::~MatrixUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void MatrixUbo::cpuUpdate( castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, Frustum const & frustum
		, castor::Point2f const & jitter )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.prvView = configuration.curView;
		configuration.prvViewProj = configuration.curViewProj;
		configuration.curView = view;
		configuration.projection = projection;
		configuration.curViewProj = projection * view;
		configuration.invProjection = projection.getInverse();
		configuration.jitter = { jitter->x, jitter->y, configuration.jitter->x, configuration.jitter->y };
		auto itSrc = frustum.getPlanes().begin();
		auto itDst = configuration.frustumPlanes.begin();

		while ( itSrc != frustum.getPlanes().end() )
		{
			*itDst = { itSrc->getNormal()->x, itSrc->getNormal()->y, itSrc->getNormal()->z, itSrc->getDistance() };
			++itSrc;
			++itDst;
		}
	}

	void MatrixUbo::cpuUpdate( castor::Matrix4x4f const & projection )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
	}

	//*********************************************************************************************
}
