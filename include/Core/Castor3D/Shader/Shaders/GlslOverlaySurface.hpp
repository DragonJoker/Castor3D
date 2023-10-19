/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslOverlaySurface_H___
#define ___C3D_GlslOverlaySurface_H___

#include "SdwModule.hpp"

#include <ShaderWriter/VecTypes/Vec3.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	template< ast::var::Flag FlagT >
	struct OverlaySurfaceT
		: public sdw::StructInstance
	{
		OverlaySurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, position{ this->getMember< sdw::Vec2 >( "position", true ) }
			, texUV{ this->getMember< sdw::Vec2 >( "texture", true ) }
			, fontUV{ this->getMember< sdw::Vec2 >( "fontUV", true ) }
			, materialId{ this->getMember< sdw::UInt >( "materialId", true ) }
		{
		}

		OverlaySurfaceT( sdw::Vec2 const pos
			, sdw::Vec2 const uv )
			: OverlaySurfaceT{ sdw::findWriterMandat( pos, uv )
				, makeInitExpr( pos, uv )
				, true }
		{
		}

		OverlaySurfaceT( sdw::Vec2 const pos
			, sdw::Vec2 const uv
			, sdw::Vec2 const text )
			: OverlaySurfaceT{ sdw::findWriterMandat( pos, uv, text )
				, makeInitExpr( pos, uv, text )
				, true }
		{
		}

		SDW_DeclStructInstance( , OverlaySurfaceT );

		static ast::expr::ExprPtr makeInitExpr( sdw::Vec2 const pos
			, sdw::Vec2 const uv )
		{
			sdw::expr::ExprList params;
			params.emplace_back( sdw::makeExpr( pos ) );
			params.emplace_back( sdw::makeExpr( uv ) );
			return sdw::makeAggrInit( makeType( sdw::findTypesCache( pos, uv ), false, true )
				, std::move( params ) );
		}

		static ast::expr::ExprPtr makeInitExpr( sdw::Vec2 const pos
			, sdw::Vec2 const uv
			, sdw::Vec2 const text )
		{
			sdw::expr::ExprList params;
			params.emplace_back( sdw::makeExpr( pos ) );
			params.emplace_back( sdw::makeExpr( uv ) );
			params.emplace_back( sdw::makeExpr( text ) );
			return sdw::makeAggrInit( makeType( sdw::findTypesCache( pos, uv ), true, true )
				, std::move( params ) );
		}

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, bool isTextOverlay
			, bool hasTextures )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
				, "C3D_OverlaySurface" );

			if ( result->empty() )
			{
				result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, true );
				result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, hasTextures );
				result->declMember( "fontUV", ast::type::Kind::eVec2F, ast::type::NotArray, isTextOverlay );
			}

			return result;
		}

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, bool hasPosition
			, bool isTextOverlay
			, bool hasTextures
			, bool isTransition )
		{
			auto result = cache.getIOStruct( "C3D_OverlaySurface"
				, entryPoint
				, FlagT );

			if ( result->empty() )
			{
				result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, 0u, hasPosition );
				result->declMember( "texture", ast::type::Kind::eVec2F, ast::type::NotArray, 1u, hasTextures );
				result->declMember( "fontUV", ast::type::Kind::eVec2F, ast::type::NotArray, 2u, isTextOverlay );
				result->declMember( "materialId", ast::type::Kind::eUInt, ast::type::NotArray, 3u, isTransition );
			}

			return result;
		}

		void set( sdw::Vec2 const & pos
			, sdw::Vec2 const & uv )
		{
			position = pos;
			texUV = uv;
		}

		void set( sdw::Vec2 const & pos
			, sdw::Vec2 const & uv
			, sdw::Vec2 const & text )
		{
			position = pos;
			texUV = uv;
			fontUV = text;
		}

		sdw::Vec2 position;
		sdw::Vec2 texUV;
		sdw::Vec2 fontUV;
		sdw::UInt materialId;
	};
}

#define C3D_OverlaysSurfaces( writer, binding, set, isText, hasTex )\
	sdw::StorageBuffer c3d_overlaysSurfacesBuffer{ writer\
		, "C3D_OverlaysSurfacesBuffer"\
		, "c3d_overlaysSurfacesBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_overlaysSurfaces = c3d_overlaysSurfacesBuffer.declMemberArray< castor3d::shader::OverlaySurface >( "d"\
		, true\
		, isText\
		, hasTex );\
	c3d_overlaysSurfacesBuffer.end()

#endif
