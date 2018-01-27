/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardUbo_H___
#define ___C3D_BillboardUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Model Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour le modèle.
	*/
	class BillboardUbo
	{
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
		C3D_API BillboardUbo & operator=( BillboardUbo && ) = default;
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
		 *\brief		Updates the UBO from given values.
		 *\param[in]	dimensions	The billboard dimensions.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	dimensions	Les dimensions du billboard.
		 */
		C3D_API void update( castor::Point2f const & dimensions )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	window	The window dimensions.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	window	Les dimensions de la fenêtre.
		 */
		C3D_API void setWindowSize( castor::Size const & window )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBuffer & getUbo()
		{
			return m_ubo;
		}

		inline UniformBuffer const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		C3D_API static constexpr uint32_t BindingPoint = 9u;
		//!\~english	Name of the billboards frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de billboards.
		C3D_API static castor::String const BufferBillboard;
		//!\~english	Name of the billboard dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions du billboard.
		C3D_API static castor::String const Dimensions;
		//!\~english	Name of the window dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions de la fenêtre.
		C3D_API static castor::String const WindowSize;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The dimensions uniform variable.
		//!\~french		La variable uniforme des dimensions.
		Uniform2f & m_dimensions;
		//!\~english	The window dimensions uniform variable.
		//!\~french		La variable uniforme des dimensions de la fenêtre.
		Uniform2i & m_windowSize;
	};
}

#define UBO_BILLBOARD( writer, set )\
	glsl::Ubo billboard{ writer\
		, castor3d::BillboardUbo::BufferBillboard\
		, castor3d::BillboardUbo::BindingPoint\
		, set\
		, glsl::Ubo::Layout::eStd140 };\
	auto c3d_dimensions = billboard.declMember< Vec2 >( castor3d::BillboardUbo::Dimensions );\
	auto c3d_windowSize = billboard.declMember< IVec2 >( castor3d::BillboardUbo::WindowSize );\
	billboard.end()

#endif
