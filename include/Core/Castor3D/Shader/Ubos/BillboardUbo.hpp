/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardUbo_H___
#define ___C3D_BillboardUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class BillboardUbo
	{
	public:
		struct Configuration
		{
			castor::Point2f dimensions;
		};

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API BillboardUbo( BillboardUbo const & ) = delete;
		C3D_API BillboardUbo & operator=( BillboardUbo const & ) = delete;
		C3D_API BillboardUbo( BillboardUbo && ) = default;
		C3D_API BillboardUbo & operator=( BillboardUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit BillboardUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BillboardUbo();
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
		 *\param[in]	dimensions	The billboard dimensions.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	dimensions	Les dimensions du billboard.
		 */
		C3D_API void update( castor::Point2f const & dimensions )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
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
		//!\~english	Name of the billboards frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de billboards.
		C3D_API static castor::String const BufferBillboard;
		//!\~english	Name of the billboard dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions du billboard.
		C3D_API static castor::String const Dimensions;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
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
