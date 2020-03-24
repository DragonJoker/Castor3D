/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingUbo_H___
#define ___C3D_MorphingUbo_H___

#include "UbosModule.hpp"

namespace castor3d
{
	class MorphingUbo
	{
	public:
		using Configuration = MorphingUboConfiguration;

	public:
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the morphing animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de morphing.
		C3D_API static castor::String const BufferMorphing;
		//!\~english	Name of the morphing time attribute.
		//!\~french		Nom de l'attribut du temps d'animation par sommet.
		C3D_API static castor::String const Time;
	};
}

#define UBO_MORPHING( writer, binding, set, flags )\
	sdw::Ubo morphing{ writer\
		, castor3d::MorphingUbo::BufferMorphing\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_time = morphing.declMember< sdw::Float >( castor3d::MorphingUbo::Time, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) );\
	morphing.end()

#endif
