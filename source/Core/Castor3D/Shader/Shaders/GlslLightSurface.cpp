#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	LightSurface::LightSurface( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, c3d::move( expr ), enabled }
		, m_eyePosition{ getMember< sdw::Vec3 >( "eyePosition" ) }
		, m_worldPosition{ getMember< DerivVec4 >( "worldPosition" ) }
		, m_viewPosition{ getMember< DerivVec3 >( "viewPosition" ) }
		, m_clipPosition{ getMember< sdw::Vec3 >( "clipPosition" ) }
		, m_vertexToLight{ getMember< DerivVec3 >( "vertexToLight" ) }
		, m_V{ getMember< DerivVec3 >( "V" ) }
		, m_N{ getMember< DerivVec3 >( "N" ) }
		, m_L{ getMember< DerivVec3 >( "L" ) }
		, m_H{ getMember< DerivVec3 >( "H" ) }
		, m_lengthV{ getMember< DerivFloat >( "lengthV" ) }
		, m_lengthL{ getMember< DerivFloat >( "lengthL" ) }
		, m_NdotV{ getMember< DerivFloat >( "NdotV" ) }
		, m_NdotL{ getMember< DerivFloat >( "NdotL", derivFloat( 0.0_f ) ) }
		, m_NdotH{ getMember< DerivFloat >( "NdotH", derivFloat( 0.0_f ) ) }
		, m_HdotV{ getMember< DerivFloat >( "HdotV", derivFloat( 0.0_f ) ) }
		, m_LdotV{ getMember< DerivFloat >( "LdotV", derivFloat( 0.0_f ) ) }
		, m_HdotL{ getMember< DerivFloat >( "HdotL", derivFloat( 0.0_f ) ) }
	{
	}

	LightSurface::LightSurface( sdw::Vec3 const & eye
		, DerivVec4 const & world
		, DerivVec3 const & view
		, sdw::Vec3 const & clip
		, DerivVec3 const & normal
		, bool enableDotProducts )
		: LightSurface{ findWriterMandat( eye, world, view, clip, normal )
			, makeInit( makeType( findTypesCache( eye, world, view, clip, normal ), enableDotProducts )
				, eye, world, view, clip, normal
				, enableDotProducts )
			, true }
	{
	}

	sdw::type::BaseStructPtr LightSurface::makeType( [[maybe_unused]] ast::type::TypesCache & cache
		, sdw::type::TypePtr type )
	{
		return static_cast< sdw::type::BaseStruct * >( type );
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, bool enableDotProducts )
	{
		auto type = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_LightSurface"
				+ ( enableDotProducts ? MbString{ "Prods" } : MbString{} ) );

		if ( type->empty() )
		{
			type->declMember( "eyePosition", sdw::Vec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "worldPosition", DerivVec4::makeType( cache ), ast::type::NotArray );
			type->declMember( "viewPosition", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "clipPosition", sdw::Vec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "vertexToLight", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "V", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "N", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "L", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "H", DerivVec3::makeType( cache ), ast::type::NotArray );
			type->declMember( "lengthV", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "lengthL", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "NdotV", DerivFloat::makeType( cache ), ast::type::NotArray );
			type->declMember( "NdotL", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "NdotH", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "HdotV", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "LdotV", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
			type->declMember( "HdotL", DerivFloat::makeType( cache ), ast::type::NotArray, enableDotProducts );
		}

		return type;
	}

	ast::type::BaseStructPtr LightSurface::makeType( [[maybe_unused]] ast::type::TypesCache & cache
		, LightSurface const & rhs )
	{
		return static_cast< ast::type::BaseStruct * >( rhs.getType() );
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, MbString const & name
		, sdw::Vec3 const & eye
		, DerivVec4 const & world
		, DerivVec3 const & view
		, sdw::Vec3 const & clip
		, DerivVec3 const & normal
		, bool enableDotProducts )
	{
		auto result = writer.declLocale< LightSurface >( name
			, LightSurface{ eye, world, view, clip, normal, enableDotProducts } );
		result.m_NdotV = max( derivFloat( 0.0_f ), dot( result.N(), result.V() ) );
		return result;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, MbString const & name
		, DerivVec4 const & world
		, sdw::Vec3 const & clip
		, DerivVec3 const & normal
		, bool enableDotProducts )
	{
		return create( writer, name, vec3( 0.0_f ), world, derivVec3( 0.0_f ), clip, normal
			, enableDotProducts );
	}

	void LightSurface::updateN( DerivVec3 const & n )const
	{
		N() = n;

		m_NdotL = clamp( dot( N(), L() ), 0.0_f, 1.0_f );
		m_NdotH = clamp( dot( N(), H() ), 0.0_f, 1.0_f );
		m_NdotV = clamp( dot( N(), V() ), 0.0_f, 1.0_f );
	}

	void LightSurface::updateL( DerivVec3 const & VtoL )const
	{
		vertexToLight() = VtoL;
		L() = normalize( vertexToLight() );
		lengthL() = length( vertexToLight() );
		H() = normalize( L() + V() );

		m_NdotL = clamp( dot( N(), L() ), 0.0_f, 1.0_f );
		m_NdotH = clamp( dot( N(), H() ), 0.0_f, 1.0_f );
		m_HdotV = clamp( dot( H(), V() ), 0.0_f, 1.0_f );
		m_LdotV = clamp( dot( L(), V() ), 0.0_f, 1.0_f );
		m_HdotL = clamp( dot( H(), L() ), 0.0_f, 1.0_f );
	}

	void LightSurface::registerDebug( DebugOutputCategory const & debugOutput )const
	{
		debugOutput.registerOutput( cuT( "V" ), m_V.value() );
		debugOutput.registerOutput( cuT( "N" ), m_N.value() );
		debugOutput.registerOutput( cuT( "L" ), m_L.value() );
		debugOutput.registerOutput( cuT( "H" ), m_H.value() );
		debugOutput.registerOutput( cuT( "NdotV" ), m_NdotV.value() );
		debugOutput.registerOutput( cuT( "NdotL" ), m_NdotL.getValue().value() );
		debugOutput.registerOutput( cuT( "NdotH" ), m_NdotH.getValue().value() );
		debugOutput.registerOutput( cuT( "HdotV" ), m_HdotV.getValue().value() );
		debugOutput.registerOutput( cuT( "LdotV" ), m_LdotV.getValue().value() );
		debugOutput.registerOutput( cuT( "HdotL" ), m_HdotL.getValue().value() );
	}

	sdw::expr::ExprPtr LightSurface::makeInit( sdw::type::BaseStructPtr type
		, sdw::Vec3 const & eye
		, DerivVec4 const & world
		, DerivVec3 const & view
		, sdw::Vec3 const & clip
		, DerivVec3 const & normal
		, bool enableDotProducts )
	{
		using shader::operator-;
		sdw::expr::ExprList inits;
		inits.push_back( makeExpr( eye ) ); // eyePosition
		inits.push_back( makeExpr( world ) ); // worldPosition
		inits.push_back( makeExpr( view ) ); // viewPosition
		inits.push_back( makeExpr( clip ) ); // clipPosition
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // vertexToLight
		inits.push_back( makeExpr( normalize( derivVec3( eye ) - getXYZ( world ) ) ) ); // V
		inits.push_back( makeExpr( normal ) ); // N
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // L
		inits.push_back( makeExpr( derivVec3( 0.0_f ) ) ); // H
		inits.push_back( makeExpr( length( derivVec3( eye ) - getXYZ( world ) ) ) ); // lengthV
		inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // lengthL
		inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotV

		if ( enableDotProducts )
		{
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotL
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // NdotH
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // HdotV
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // LdotV
			inits.push_back( makeExpr( derivFloat( 0.0_f ) ) ); // HdotL
		}

		return sdw::makeAggrInit( type, c3d::move( inits ) );
	}
}
