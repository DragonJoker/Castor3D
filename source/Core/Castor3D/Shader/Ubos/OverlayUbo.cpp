#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		OverlayData::OverlayData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_positionRatio{ getMember< sdw::Vec4 >( "positionRatio" ) }
			, m_renderSizeIndex{ getMember< sdw::IVec4 >( "renderSizeIndex" ) }
		{
		}

		ast::type::BaseStructPtr OverlayData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_OverlayData" );

			if ( result->empty() )
			{
				result->declMember( "positionRatio", ast::type::Kind::eVec4F );
				result->declMember( "renderSizeIndex", ast::type::Kind::eVec4I );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > OverlayData::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		sdw::Vec2 OverlayData::getOverlaySize()const
		{
			return vec2( m_positionRatio.z() * getWriter()->cast< sdw::Float >( m_renderSizeIndex.x() )
				, m_positionRatio.w() * getWriter()->cast< sdw::Float >( m_renderSizeIndex.y() ) );
		}

		sdw::Vec2 OverlayData::modelToView( sdw::Vec2 const & pos )const
		{
			return m_positionRatio.xy() + pos;
		}

		sdw::UInt OverlayData::getMaterialIndex()const
		{
			return getWriter()->cast< sdw::UInt >( m_renderSizeIndex.z() );
		}
	}

	//*********************************************************************************************

	castor::String const OverlayUbo::BufferOverlay = cuT( "Overlay" );
	castor::String const OverlayUbo::OverlayData = cuT( "c3d_overlayData" );

	//*********************************************************************************************
}
