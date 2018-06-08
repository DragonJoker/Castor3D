/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMatrixUbo_H___
#define ___C3D_ModelMatrixUbo_H___

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Utils/Mat4.hpp>

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
	class ModelMatrixUbo
	{
	public:
		struct Configuration
		{
			castor::Matrix4x4f model;
			castor::Matrix4x4f normal;
		};

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API ModelMatrixUbo( ModelMatrixUbo const & ) = delete;
		C3D_API ModelMatrixUbo & operator=( ModelMatrixUbo const & ) = delete;
		C3D_API ModelMatrixUbo( ModelMatrixUbo && ) = default;
		C3D_API ModelMatrixUbo & operator=( ModelMatrixUbo && ) = default;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ModelMatrixUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelMatrixUbo();
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
		 *\param[in]	model	The new model matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	model	La nouvelle matrice modèle.
		 */
		C3D_API void update( castor::Matrix4x4r const & model )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	model	The new model matrix.
		 *\param[in]	normal	The new normal matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	model	La nouvelle matrice modèle.
		 *\param[in]	normal	La nouvelle matrice normale.
		 */
		C3D_API void update( castor::Matrix4x4r const & model
			, castor::Matrix3x3r const & normal )const;
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline renderer::UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline renderer::UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static const uint32_t BindingPoint;
		C3D_API static castor::String const BufferModelMatrix;
		C3D_API static castor::String const MtxModel;
		C3D_API static castor::String const MtxNormal;

	private:
		Engine & m_engine;
		renderer::UniformBufferPtr< Configuration > m_ubo;
	};
}

#define UBO_MODEL_MATRIX( writer, binding, set )\
	glsl::Ubo modelMatrices{ writer\
		, castor3d::ModelMatrixUbo::BufferModelMatrix\
		, binding\
		, set\
		, glsl::Ubo::Layout::eStd140 };\
	auto c3d_mtxModel = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxModel );\
	auto c3d_mtxNormal = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxNormal );\
	modelMatrices.end()

#endif
