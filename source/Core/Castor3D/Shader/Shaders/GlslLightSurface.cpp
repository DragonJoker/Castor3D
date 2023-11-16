#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	LightSurface::LightSurface( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, std::move( expr ), enabled }
		, m_eyePosition{ getMember< sdw::Vec3 >( "eyePosition" ) }
		, m_worldPosition{ getMember< sdw::Vec4 >( "worldPosition" ) }
		, m_viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
		, m_clipPosition{ getMember< sdw::Vec3 >( "clipPosition" ) }
		, m_vertexToLight{ getMember< sdw::Vec3 >( "vertexToLight" ) }
		, m_V{ getMember< sdw::Vec3 >( "V" ) }
		, m_N{ getMember< sdw::Vec3 >( "N" ) }
		, m_L{ getMember< sdw::Vec3 >( "L" ) }
		, m_H{ getMember< sdw::Vec3 >( "H" ) }
		, m_lengthV{ getMember< sdw::Float >( "lengthV" ) }
		, m_lengthL{ getMember< sdw::Float >( "lengthL" ) }
		, m_NdotV{ getMember< sdw::Float >( "NdotV" ) }
		, m_NdotL{ getMember< sdw::Float >( "NdotL", 0.0_f ) }
		, m_NdotH{ getMember< sdw::Float >( "NdotH", 0.0_f ) }
		, m_HdotV{ getMember< sdw::Float >( "HdotV", 0.0_f ) }
		, m_F{ getMember< sdw::Vec3 >( "F", vec3( 0.0_f ) ) }
		, m_spcF{ getMember< sdw::Vec3 >( "spcF", m_F ) }
		, m_difF{ getMember< sdw::Vec3 >( "difF", m_F ) }
	{
	}

	LightSurface::LightSurface( sdw::Vec3 const eye
		, sdw::Vec4 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
		: LightSurface{ findWriterMandat( eye, world, view, clip, normal )
			, makeInit( makeType( findTypesCache( eye, world, view, clip, normal ), enableDotProducts, enableFresnel, enableIridescence )
				, eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence )
			, true }
	{
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, sdw::type::TypePtr type )
	{
		return std::static_pointer_cast< sdw::type::BaseStruct >( type );
	}

	sdw::type::BaseStructPtr LightSurface::makeType( ast::type::TypesCache & cache
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto type = cache.getStruct( ast::type::MemoryLayout::eC
			, "C3D_LightSurface"
				+ ( enableDotProducts ? std::string{ "Prods" } : std::string{} )
				+ ( enableFresnel ? std::string{ "F" } : std::string{} )
				+ ( enableIridescence ? std::string{ "I" } : std::string{} ) );

		if ( type->empty() )
		{
			type->declMember( "eyePosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "worldPosition", ast::type::Kind::eVec4F, ast::type::NotArray );
			type->declMember( "viewPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "clipPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "vertexToLight", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "V", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "N", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "L", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "H", ast::type::Kind::eVec3F, ast::type::NotArray );
			type->declMember( "lengthV", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "lengthL", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "NdotV", ast::type::Kind::eFloat, ast::type::NotArray );
			type->declMember( "NdotL", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "NdotH", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "HdotV", ast::type::Kind::eFloat, ast::type::NotArray, enableDotProducts );
			type->declMember( "F", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel );
			type->declMember( "spcF", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel && enableIridescence );
			type->declMember( "difF", ast::type::Kind::eVec3F, ast::type::NotArray, enableFresnel && enableIridescence );
		}

		return type;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, std::string const & name
		, sdw::Vec3 const eye
		, sdw::Vec4 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = writer.declLocale< LightSurface >( name
			, LightSurface{ eye, world, view, clip, normal
				, enableDotProducts, enableFresnel, enableIridescence } );
		result.m_NdotV = max( 0.0_f, dot( result.N(), result.V() ) );
		return result;
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, std::string const & name
		, sdw::Vec4 const world
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		return create( writer, name, vec3( 0.0_f ), world, vec3( 0.0_f ), clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
	}

	LightSurface LightSurface::create( sdw::ShaderWriter & writer
		, Utils & utils
		, std::string const & name
		, sdw::Vec3 const eye
		, sdw::Vec4 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		auto result = create( writer, name, eye, world, view, clip, normal
			, enableDotProducts, enableFresnel, enableIridescence );
		result.doUpdateF( utils, f0, components, result.NdotV() );
		return result;
	}

	void LightSurface::updateN( sdw::Vec3 const n )const
	{
		N() = n;

		m_NdotL = max( 0.0_f, dot( N(), L() ) );
		m_NdotH = max( 0.0_f, dot( N(), H() ) );
		m_NdotV = max( 0.0_f, dot( N(), V() ) );
	}

	void LightSurface::updateL( sdw::Vec3 const VtoL )const
	{
		vertexToLight() = VtoL;
		L() = normalize( vertexToLight() );
		lengthL() = length( vertexToLight() );
		H() = normalize( L() + V() );

		m_NdotL = max( 0.0_f, dot( N(), L() ) );
		m_NdotH = max( 0.0_f, dot( N(), H() ) );
		m_HdotV = max( 0.0_f, dot( H(), V() ) );
	}

	void LightSurface::updateN( Utils & utils
		, sdw::Vec3 const n
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateN( n );
		doUpdateF( utils, f0, components, NdotV() );
	}

	void LightSurface::updateL( Utils & utils
		, sdw::Vec3 const VtoL
		, sdw::Vec3 const f0
		, BlendComponents const & components )const
	{
		updateL( VtoL );
		doUpdateF( utils, f0, components, HdotV() );
	}

	void LightSurface::doUpdateF( Utils & utils
		, sdw::Vec3 const f0
		, BlendComponents const & components
		, sdw::Float const & dotProduct )const
	{
		m_F = utils.conductorFresnel( dotProduct
			, f0
			, components.f90 );

		if ( m_difF )
		{
			auto & writer = *getWriter();

			IF( writer, components.iridescenceFactor != 0.0_f )
			{
				// Blend default specular Fresnel with iridescence Fresnel
				m_spcF = mix( F(), components.iridescenceFresnel, vec3( components.iridescenceFactor ) );
				// Use the maximum component of the iridescence Fresnel color
				// Maximum is used instead of the RGB value to not get inverse colors for the diffuse BRDF
				m_difF = mix( F()
					, vec3( max( max( components.iridescenceFresnel.r(), components.iridescenceFresnel.g() ), components.iridescenceFresnel.b() ) )
					, vec3( components.iridescenceFactor ) );
			}
			ELSE
			{
				m_spcF = F();
				m_difF = F();
			}
			FI;
		}
	}

	sdw::expr::ExprPtr LightSurface::makeInit( sdw::type::BaseStructPtr type
		, sdw::Vec3 const eye
		, sdw::Vec4 const world
		, sdw::Vec3 const view
		, sdw::Vec3 const clip
		, sdw::Vec3 const normal
		, bool enableDotProducts
		, bool enableFresnel
		, bool enableIridescence )
	{
		sdw::expr::ExprList inits;
		inits.push_back( makeExpr( eye ) ); // eyePosition
		inits.push_back( makeExpr( world ) ); // worldPosition
		inits.push_back( makeExpr( view ) ); // viewPosition
		inits.push_back( makeExpr( clip ) ); // clipPosition
		inits.push_back( makeExpr( vec3(  0.0_f ) ) ); // vertexToLight
		inits.push_back( makeExpr( normalize( eye - world.xyz() ) ) ); // V
		inits.push_back( makeExpr( normal ) ); // N
		inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // L
		inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // H
		inits.push_back( makeExpr( length( eye - world.xyz() ) ) ); // lengthV
		inits.push_back( makeExpr( 0.0_f ) ); // lengthL
		inits.push_back( makeExpr( 0.0_f ) ); // NdotV

		if ( enableDotProducts )
		{
			inits.push_back( makeExpr( 0.0_f ) ); // NdotL
			inits.push_back( makeExpr( 0.0_f ) ); // NdotH
			inits.push_back( makeExpr( 0.0_f ) ); // HdotV
		}

		if ( enableFresnel )
		{
			inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // F

			if ( enableIridescence )
			{
				inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // spcF
				inits.push_back( makeExpr( vec3( 0.0_f ) ) ); // difF
			}
		}

		return sdw::makeAggrInit( type, std::move( inits ) );
	}
}
