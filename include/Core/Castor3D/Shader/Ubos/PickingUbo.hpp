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
		struct Configuration
		{
			uint32_t drawIndex;
			uint32_t nodeIndex;
		};

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API PickingUbo( PickingUbo const & ) = delete;
		C3D_API PickingUbo & operator=( PickingUbo const & ) = delete;
		C3D_API PickingUbo( PickingUbo && ) = default;
		C3D_API PickingUbo & operator=( PickingUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit PickingUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PickingUbo();
		/**
		 *\~english
		 *\brief		Initialises the UBO.
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
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
		C3D_API void update( uint32_t drawIndex
			, uint32_t nodeIndex )const;
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
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
		inline UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

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
