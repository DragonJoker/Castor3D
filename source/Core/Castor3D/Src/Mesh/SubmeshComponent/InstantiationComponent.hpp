/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiationComponent_H___
#define ___C3D_InstantiationComponent_H___

#include "SubmeshComponent.hpp"

#include <Utils/Mat4.hpp>

namespace castor3d
{
	struct InstantiationData
	{
		renderer::Mat4 m_matrix;
		int m_material;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		The submesh component used for hardware instantiation.
	\~french
	\brief		Le composant de sous-maillage pour l'instantiation matérielle.
	*/
	class InstantiationComponent
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh		The parent submesh.
		 *\param[in]	threshold	The threshold from which a submesh will be instantiated.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh		Le sous-maillage parent.
		 *\param[in]	threshold	Le seuil à partir duquel un sous-maillage sera instancié.
		 */
		C3D_API explicit InstantiationComponent( Submesh & submesh
			, uint32_t threshold = 1u );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~InstantiationComponent();
		/**
		 *\~english
		 *\brief		Increments instance count.
		 *\param[in]	material	The material for which the instance count is incremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Incrémente le compte d'instances.
		 *\param[in]	material	Le matériau pour lequel le compte est incrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t ref( MaterialSPtr material );
		/**
		 *\~english
		 *\brief		Decrements instance count.
		 *\param[in]	material	The material for which the instance count is decremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Décrémente le compte d'instances.
		 *\param[in]	material	Le matériau pour lequel le compte est décrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t unref( MaterialSPtr material );
		/**
		 *\~english
		 *\brief		Retrieves the instances count
		 *\param[in]	material	The material for which the instance count is retrieved
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'instances
		 *\param[in]	material	Le matériau pour lequel le compte est récupéré
		 *\return		La valeur
		 */
		C3D_API uint32_t getRefCount( MaterialSPtr material )const;
		/**
		 *\~english
		 *\return		The maximum instances count, amongst all materials.
		 *\~french
		 *\return		Le nombre moximum d'instances, tous matériaux confondus,
		 */
		C3D_API uint32_t getMaxRefCount()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( renderer::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, renderer::VertexLayoutCRefArray & layouts )override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::setMaterial
		 */
		C3D_API void setMaterial( MaterialSPtr oldMaterial
			, MaterialSPtr newMaterial
			, bool update )override;
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline bool hasMatrixBuffer()const
		{
			return bool( m_matrixBuffer );
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline renderer::VertexBuffer< InstantiationData > const & getMatrixBuffer()const
		{
			return *m_matrixBuffer;
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline renderer::VertexBuffer< InstantiationData > & getMatrixBuffer()
		{
			return *m_matrixBuffer;
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline std::vector< InstantiationData > & getData()
		{
			return m_data;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return hasMatrixBuffer()
				? ProgramFlag::eInstantiation
				: ProgramFlag( 0 );
		}

	private:
		bool doInitialise()override;
		void doCleanup()override;
		void doFill()override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 2u;

	private:
		std::map< MaterialSPtr, uint32_t > m_instanceCount;
		renderer::VertexBufferPtr< InstantiationData > m_matrixBuffer;
		renderer::VertexLayoutPtr m_matrixLayout;
		std::vector< InstantiationData > m_data;
		uint32_t m_threshold;
	};
}

#endif
