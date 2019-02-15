/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelUbo_H___
#define ___C3D_ModelUbo_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

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
		struct Configuration
		{
			int32_t shadowReceiver;
			int32_t materialIndex;
			int32_t environmentIndex;
		};

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
		 *\param[in]	shadowReceiver	Tells if the model receives shadows.
		 *\param[in]	materialIndex	The material index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	shadowReceiver	Dit si le modèle reçoit les ombres.
		 *\param[in]	materialIndex	L'indice du matériau.
		 */
		C3D_API void update( bool shadowReceiver
			, uint32_t materialIndex )const;
		/**
		 *\~english
		 *\brief		Sets the environment map index value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de l'indice de la texture d'environnement.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setEnvMapIndex( uint32_t value );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
		inline ashes::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline ashes::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static uint32_t const BindingPoint;
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
		Engine & m_engine;
		ashes::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_MODEL( writer, binding, set )\
	sdw::Ubo model{ writer\
		, castor3d::ModelUbo::BufferModel\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_shadowReceiver = model.declMember< sdw::Int >( castor3d::ModelUbo::ShadowReceiver );\
	auto c3d_materialIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::MaterialIndex );\
	auto c3d_envMapIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::EnvironmentIndex );\
	model.end()

#endif
