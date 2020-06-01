/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickingUbo_H___
#define ___C3D_PickingUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class PickingUbo
	{
	public:
		using Configuration = PickingUboConfiguration;
		PickingUbo( Engine & engine );
		void initialise();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	drawIndex	The draw call index.
		 *\param[in]	nodeIndex	The node index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	drawIndex	L'indice de la fonction de dessin.
		 *\param[in]	nodeIndex	L'indice du noeud.
		 */
		C3D_API static void update( Configuration & configuration
			, uint32_t drawIndex
			, uint32_t nodeIndex );

	public:
		C3D_API static const uint32_t BindingPoint;
		//!\~english	Name of the picking information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de picking.
		C3D_API static castor::String const BufferPicking;
		//!\~english	Name of the draw index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du dessin.
		C3D_API static castor::String const DrawIndex;
		//!\~english	Name of the node index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du noeud.
		C3D_API static castor::String const NodeIndex;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_PICKING( writer, binding, set )\
	sdw::Ubo model{ writer\
		, castor3d::PickingUbo::BufferPicking\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_drawIndex = model.declMember< sdw::Int >( castor3d::PickingUbo::DrawIndex );\
	auto c3d_nodeIndex = model.declMember< sdw::Int >( castor3d::PickingUbo::NodeIndex );\
	model.end()

#endif
