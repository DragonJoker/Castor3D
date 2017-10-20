/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "Shader/UniformBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		12/04/2017
	\~english
	\brief		Matrices Uniform buffer management.
	\~french
	\brief		Gestion du tampon de variables uniformes pour les matrices.
	*/
	class OverlayUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API OverlayUbo( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo & operator=( OverlayUbo const & ) = delete;
		C3D_API OverlayUbo( OverlayUbo && ) = default;
		C3D_API OverlayUbo & operator=( OverlayUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API OverlayUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OverlayUbo();
		/**
		 *\~english
		 *\brief		sets the overlay position value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la position de l'incrustation.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void setPosition( castor::Position const & p_position );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_materialIndex	The overlay's material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_materialIndex	L'index du matériau de l'incrustation.
		 */
		C3D_API void update( int p_materialIndex )const;
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
		static constexpr uint32_t BindingPoint = 2u;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static castor::String const BufferOverlay;
		//!\~english	Name of the overlay position frame variable.
		//!\~french		Nom de la frame variable contenant la position de l'incrustation.
		C3D_API static castor::String const Position;
		//!\~english	Name of the material index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du matériau.
		C3D_API static castor::String const MaterialIndex;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The uniform variable containing overlay position.
		//!\~french		La variable uniforme contenant la position de l'incrustation.
		Uniform2i & m_position;
		//!\~english	The uniform variable containing overlay's material index.
		//!\~french		La variable uniforme contenant l'indice du matériau de l'incrustation.
		Uniform1i & m_material;
	};
}

#define UBO_OVERLAY( Writer )\
	glsl::Ubo overlay{ writer, castor3d::OverlayUbo::BufferOverlay, castor3d::OverlayUbo::BindingPoint };\
	auto c3d_position = overlay.declMember< glsl::IVec2 >( castor3d::OverlayUbo::Position );\
	auto c3d_materialIndex = overlay.declMember< glsl::Int >( castor3d::OverlayUbo::MaterialIndex );\
	overlay.end()

#endif
