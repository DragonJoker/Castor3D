/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardUbo_H___
#define ___C3D_BillboardUbo_H___

#include "UbosModule.hpp"

namespace castor3d
{
	class BillboardUbo
	{
	public:
		using Configuration = BillboardUboConfiguration;

	public:
		C3D_API static const uint32_t BindingPoint;
		//!\~english	Name of the billboards frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de billboards.
		C3D_API static castor::String const BufferBillboard;
		//!\~english	Name of the billboard dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions du billboard.
		C3D_API static castor::String const Dimensions;
	};
}

#define UBO_BILLBOARD( writer, binding, set )\
	sdw::Ubo billboard{ writer\
		, castor3d::BillboardUbo::BufferBillboard\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_dimensions = billboard.declMember< Vec2 >( castor3d::BillboardUbo::Dimensions );\
	billboard.end()

#endif
