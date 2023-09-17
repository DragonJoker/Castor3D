/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslAABB_H___
#define ___C3D_GlslAABB_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct Plane
		: public sdw::StructInstanceHelperT< "C3D_Plane"
		, sdw::type::MemoryLayout::eC
		, sdw::Vec3Field< "normal" >
		, sdw::FloatField< "distance" > >
	{
		Plane( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		Plane( sdw::Vec3 const normal
			, sdw::Float const distance
			, bool enabled = true )
			: Plane{ sdw::findWriterMandat( normal, distance )
				, sdw::makeAggrInit( makeType( sdw::findTypesCache( normal, distance ) )
					, makeExprList( sdw::makeExpr( normal ), sdw::makeExpr( distance ) ) )
				, enabled }
		{
		}

		auto normal()const { return getMember< "normal" >(); }
		auto distance()const { return getMember< "distance" >(); }
	};

	struct Cone
		: public sdw::StructInstanceHelperT< "C3D_Cone"
		, sdw::type::MemoryLayout::eC
		, sdw::Vec3Field< "apex" >
		, sdw::FloatField< "range" >
		, sdw::Vec3Field< "direction" >
		, sdw::FloatField< "apertureCos" >
		, sdw::FloatField< "apertureSin" >
		, sdw::FloatField< "apertureTan" > >
	{
		Cone( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		Cone( sdw::Vec3 const papex
			, sdw::Vec3 const pdirection
			, sdw::Float const prange
			, sdw::Float const papertureCos
			, sdw::Float const papertureSin
			, sdw::Float const papertureTan
			, bool enabled = true )
			: Cone{ sdw::findWriterMandat( papex, prange, pdirection, papertureCos, papertureSin, papertureTan )
				, sdw::makeAggrInit( makeType( sdw::findTypesCache( papex, prange, pdirection, papertureCos, papertureSin, papertureTan ) )
					, makeExprList( sdw::makeExpr( papex ), sdw::makeExpr( prange ), sdw::makeExpr( pdirection )
						, sdw::makeExpr( papertureCos ), sdw::makeExpr( papertureSin ), sdw::makeExpr( papertureTan ) ) )
				, enabled }
		{
		}

		auto apex()const { return getMember< "apex" >(); }
		auto range()const { return getMember< "range" >(); }
		auto direction()const { return getMember< "direction" >(); }
		auto apertureCos()const { return getMember< "apertureCos" >(); }
		auto apertureSin()const { return getMember< "apertureSin" >(); }
		auto apertureTan()const { return getMember< "apertureTan" >(); }
	};

	struct AABB
		: public sdw::StructInstanceHelperT< "C3D_AABB"
		, sdw::type::MemoryLayout::eStd430
		, sdw::Vec4Field< "bmin" >
		, sdw::Vec4Field< "bmax" > >
	{
		AABB( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		AABB( sdw::Vec4 const pmin
			, sdw::Vec4 const pmax
			, bool enabled = true )
			: AABB{ sdw::findWriterMandat( pmin, pmax )
				, sdw::makeAggrInit( makeType( sdw::findTypesCache( pmin, pmax ) )
					, makeExprList( sdw::makeExpr( pmin ), sdw::makeExpr( pmax ) ) )
				, enabled }
		{
		}

		auto min()const { return getMember< "bmin" >(); }
		auto max()const { return getMember< "bmax" >(); }
	};
}

#endif
