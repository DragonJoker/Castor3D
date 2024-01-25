/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLightSurface_H___
#define ___C3D_GlslLightSurface_H___

#include "SdwModule.hpp"

#include <ShaderWriter/VecTypes/Vec3.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	struct LightSurface
		: public sdw::StructInstance
	{
		C3D_API LightSurface( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API LightSurface( sdw::Vec3 const eye
			, sdw::Vec4 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );

		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, sdw::type::TypePtr type );
		C3D_API static sdw::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );

		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, castor::MbString const & name
			, sdw::Vec3 const eye
			, sdw::Vec4 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts = true
			, bool enableFresnel = true
			, bool enableIridescence = true );
		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, castor::MbString const & name
			, sdw::Vec4 const world
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts = false
			, bool enableFresnel = false
			, bool enableIridescence = false );
		C3D_API static LightSurface create( sdw::ShaderWriter & writer
			, Utils & utils
			, castor::MbString const & name
			, sdw::Vec3 const eye
			, sdw::Vec4 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, sdw::Vec3 const f0
			, BlendComponents const & components
			, bool enableDotProducts = true
			, bool enableFresnel = true
			, bool enableIridescence = true );

		C3D_API void updateN( sdw::Vec3 const N )const;
		C3D_API void updateL( sdw::Vec3 const VtoL )const;

		C3D_API void updateN( Utils & utils
			, sdw::Vec3 const N
			, sdw::Vec3 const f0
			, BlendComponents const & components )const;
		C3D_API void updateL( Utils & utils
			, sdw::Vec3 const VtoL
			, sdw::Vec3 const f0
			, BlendComponents const & components )const;

		auto eyePosition()const { return m_eyePosition; }
		auto worldPosition()const { return m_worldPosition; }
		auto viewPosition()const { return m_viewPosition; }
		auto clipPosition()const { return m_clipPosition; }
		auto vertexToLight()const { return m_vertexToLight; }
		auto V()const { return m_V; }
		auto N()const { return m_N; }
		auto L()const { return m_L; }
		auto H()const { return m_H; }
		auto lengthV()const { return m_lengthV; }
		auto lengthL()const { return m_lengthL; }
		auto NdotV()const { return m_NdotV; }
		sdw::Float NdotL()const { return m_NdotL; }
		sdw::Float NdotH()const { return m_NdotH; }
		sdw::Vec3 F()const { return m_F; }
		sdw::Vec3 spcF()const { return m_spcF; }
		sdw::Vec3 difF()const { return m_difF; }

	private:
		sdw::Vec3 m_eyePosition;
		sdw::Vec4 m_worldPosition;
		sdw::Vec3 m_viewPosition;
		sdw::Vec3 m_clipPosition;
		sdw::Vec3 m_vertexToLight;
		sdw::Vec3 m_V;
		sdw::Vec3 m_N;
		sdw::Vec3 m_L;
		sdw::Vec3 m_H;
		sdw::Float m_lengthV;
		sdw::Float m_lengthL;
		mutable sdw::Float m_NdotV;

		mutable sdw::DefaultedT< sdw::Float > m_NdotL;
		mutable sdw::DefaultedT< sdw::Float > m_NdotH;
		mutable sdw::DefaultedT< sdw::Float > m_HdotV;

		mutable sdw::DefaultedT< sdw::Vec3 > m_F;
		mutable sdw::DefaultedT< sdw::Vec3 > m_spcF;
		mutable sdw::DefaultedT< sdw::Vec3 > m_difF;

	private:
		auto HdotV()const { return m_HdotV; }

		C3D_API void doUpdateF( Utils & utils
			, sdw::Vec3 const f0
			, BlendComponents const & components
			, sdw::Float const & dotProduct )const;
		C3D_API static sdw::expr::ExprPtr makeInit( sdw::type::BaseStructPtr type
			, sdw::Vec3 const eye
			, sdw::Vec4 const world
			, sdw::Vec3 const view
			, sdw::Vec3 const clip
			, sdw::Vec3 const normal
			, bool enableDotProducts
			, bool enableFresnel
			, bool enableIridescence );
	};
}

#endif
