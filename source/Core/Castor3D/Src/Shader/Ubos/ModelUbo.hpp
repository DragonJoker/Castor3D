/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelUbo_H___
#define ___C3D_ModelUbo_H___

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
	class ModelUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API ModelUbo( ModelUbo const & ) = delete;
		C3D_API ModelUbo & operator=( ModelUbo const & ) = delete;
		C3D_API ModelUbo( ModelUbo && ) = default;
		C3D_API ModelUbo & operator=( ModelUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ModelUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_shadowReceiver	Tells if the model receives shadows.
		 *\param[in]	p_materialIndex		The material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_shadowReceiver	Dit si le modèle reçoit les ombres.
		 *\param[in]	p_materialIndex		L'indice du matériau.
		 */
		C3D_API void update( bool p_shadowReceiver
			, uint32_t p_materialIndex )const;
		/**
		 *\~english
		 *\brief		Sets the environment map index value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'indice de la texture d'environnement.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void setEnvMapIndex( uint32_t p_value );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
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
		static constexpr uint32_t BindingPoint = 4u;
		//!\~english	Name of the model frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de modèle.
		C3D_API static castor::String const BufferModel;
		//!\~english	Name of the shadow receiver status frame variable.
		//!\~french		Nom de la frame variable contenant le statut de réception d'ombres.
		C3D_API static castor::String const ShadowReceiver;
		//!\~english	Name of the material index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du matériau.
		C3D_API static castor::String const MaterialIndex;
		//!\~english	Name of the material environment index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice de la map d'environnement du matériau.
		C3D_API static castor::String const EnvironmentIndex;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The shadow receiver matrix variable.
		//!\~french		La variable de la réception d'ombres.
		Uniform1i & m_shadowReceiver;
		//!\~english	The material index matrix variable.
		//!\~french		La variable de l'indice du matériau.
		Uniform1i & m_materialIndex;
		//!\~english	The environment map index matrix variable.
		//!\~french		La variable de l'indice de la texture d'environnement.
		Uniform1i & m_environmentIndex;
	};
}

#define UBO_MODEL( writer, set )\
	glsl::Ubo model{ writer\
		, castor3d::ModelUbo::BufferModel\
		, castor3d::ModelUbo::BindingPoint\
		, set\
		, glsl::Ubo::Layout::ePushConstants };\
	auto c3d_shadowReceiver = model.declMember< glsl::Int >( castor3d::ModelUbo::ShadowReceiver );\
	auto c3d_materialIndex = model.declMember< glsl::Int >( castor3d::ModelUbo::MaterialIndex );\
	auto c3d_envMapIndex = model.declMember< glsl::Int >( castor3d::ModelUbo::EnvironmentIndex );\
	model.end()

#endif
