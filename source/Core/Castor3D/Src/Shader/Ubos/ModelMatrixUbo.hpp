/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMatrixUbo_H___
#define ___C3D_ModelMatrixUbo_H___

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
	class ModelMatrixUbo
	{
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
		C3D_API ModelMatrixUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelMatrixUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_model			The new model matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_model			La nouvelle matrice modèle.
		 */
		C3D_API void update( castor::Matrix4x4r const & p_model )const;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	p_model			The new model matrix.
		 *\param[in]	p_projection	The new normal matrix.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	p_model			La nouvelle matrice modèle.
		 *\param[in]	p_projection	La nouvelle matrice normale.
		 */
		C3D_API void update( castor::Matrix4x4r const & p_model
			, castor::Matrix3x3r const & p_normal )const;
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
		C3D_API static castor::String const BufferModelMatrix;
		C3D_API static castor::String const MtxModel;
		C3D_API static castor::String const MtxNormal;

	private:
		//!\~english	The UBO.
		//!\~french		L'UBO.
		UniformBuffer m_ubo;
		//!\~english	The view matrix variable.
		//!\~french		La variable de la matrice vue.
		Uniform4x4f & m_model;
		//!\~english	The projection matrix variable.
		//!\~french		La variable de la matrice projection.
		Uniform4x4f & m_normal;
	};
}

#define UBO_MODEL_MATRIX( Writer )\
	glsl::Ubo modelMatrices{ writer, castor3d::ModelMatrixUbo::BufferModelMatrix, castor3d::ModelMatrixUbo::BindingPoint };\
	auto c3d_mtxModel = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxModel );\
	auto c3d_mtxNormal = modelMatrices.declMember< glsl::Mat4 >( castor3d::ModelMatrixUbo::MtxNormal );\
	modelMatrices.end()

#endif
