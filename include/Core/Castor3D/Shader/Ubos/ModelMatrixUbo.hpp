/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMatrixUbo_H___
#define ___C3D_ModelMatrixUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class ModelMatrixUbo
	{
	public:
		using Configuration = ModelMatrixUboConfiguration;

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
		C3D_API ModelMatrixUbo & operator=( ModelMatrixUbo && ) = delete;
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
		C3D_API void update( castor::Matrix4x4f const & model );
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
		C3D_API void update( castor::Matrix4x4f const & model
			, castor::Matrix3x3f const & normal );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBufferOffset< Configuration > & getUbo()
		{
			return m_ubo;
		}

		inline UniformBufferOffset< Configuration > const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		C3D_API static const uint32_t BindingPoint;
		C3D_API static castor::String const BufferModelMatrix;
		C3D_API static castor::String const PrvMtxModel;
		C3D_API static castor::String const PrvMtxNormal;
		C3D_API static castor::String const CurMtxModel;
		C3D_API static castor::String const CurMtxNormal;

	private:
		Engine & m_engine;
		UniformBufferOffset< Configuration > m_ubo;
	};
}

#define UBO_MODEL_MATRIX( writer, binding, set )\
	sdw::Ubo modelMatrices{ writer\
		, castor3d::ModelMatrixUbo::BufferModelMatrix\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_prvMtxModel = modelMatrices.declMember< sdw::Mat4 >( castor3d::ModelMatrixUbo::PrvMtxModel );\
	auto c3d_prvMtxNormal = modelMatrices.declMember< sdw::Mat4 >( castor3d::ModelMatrixUbo::PrvMtxNormal );\
	auto c3d_curMtxModel = modelMatrices.declMember< sdw::Mat4 >( castor3d::ModelMatrixUbo::CurMtxModel );\
	auto c3d_curMtxNormal = modelMatrices.declMember< sdw::Mat4 >( castor3d::ModelMatrixUbo::CurMtxNormal );\
	modelMatrices.end()

#endif
