/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslRay_H___
#define ___C3D_GlslRay_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		struct Intersection
			: public sdw::StructInstanceHelperT < "Intersection"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "point" >
			, sdw::BooleanField< "valid" >
			, sdw::FloatField< "t" > >
		{
			C3D_API Intersection( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			C3D_API static Intersection create( std::string const & name
				, sdw::ShaderWriter & writer );

			auto point()const
			{
				return getMember< "point" >();
			}

			auto valid()const
			{
				return getMember< "valid" >();
			}

			auto t()const
			{
				return getMember< "t" >();
			}
		};

		Writer_Parameter( Intersection );

		struct Ray
			: public sdw::StructInstanceHelperT < "Ray"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "origin" >
			, sdw::Vec3Field< "direction" > >
		{
			C3D_API Ray( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
				, origin{ getMember< "origin" >() }
				, direction{ getMember< "direction" >() }
			{
			}

			C3D_API static Ray create( std::string const & name
				, sdw::ShaderWriter & writer
				, sdw::Vec3 const & o = vec3( 0.0_f )
				, sdw::Vec3 const & d = vec3( 0.0_f ) );

			C3D_API sdw::Vec3 step( sdw::Float const & t )const;

			sdw::Vec3 origin;
			sdw::Vec3 direction;
		};

		Writer_Parameter( Ray );
	}
}

#endif
