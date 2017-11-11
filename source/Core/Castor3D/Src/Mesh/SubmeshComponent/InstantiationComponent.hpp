/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantiationComponent_H___
#define ___C3D_InstantiationComponent_H___

#include "SubmeshComponent.hpp"

namespace castor3d
{
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
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API InstantiationComponent( Submesh & submesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~InstantiationComponent();
		/**
		 *\~english
		 *\brief		Recreates the Matrix buffers.
		 *\~french
		 *\brief		Recrée le tampon de matrices.
		 */
		C3D_API void resetMatrixBuffers();
		/**
		 *\~english
		 *\brief		Increments instance count.
		 *\param[in]	p_material	The material for which the instance count is incremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Incrémente le compte d'instances.
		 *\param[in]	p_material	Le matériau pour lequel le compte est incrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t ref( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Decrements instance count.
		 *\param[in]	p_material	The material for which the instance count is decremented.
		 *\return		The previous instance count.
		 *\~french
		 *\brief		Décrémente le compte d'instances.
		 *\param[in]	p_material	Le matériau pour lequel le compte est décrémenté.
		 *\return		Le compte précédent.
		 */
		C3D_API uint32_t unref( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Retrieves the instances count
		 *\param[in]	p_material	The material for which the instance count is retrieved
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'instances
		 *\param[in]	p_material	Le matériau pour lequel le compte est récupéré
		 *\return		La valeur
		 */
		C3D_API uint32_t getRefCount( MaterialSPtr p_material )const;
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
		C3D_API void gather( VertexBufferArray & buffers )override;
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
		inline VertexBuffer const & getMatrixBuffer()const
		{
			return *m_matrixBuffer;
		}
		/**
		 *\~english
		 *\return		The instantiation VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer d'instanciation.
		 */
		inline VertexBuffer & getMatrixBuffer()
		{
			return *m_matrixBuffer;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return hasMatrixBuffer() ? ProgramFlag::eInstantiation : ProgramFlag( 0 );
		}

	private:
		bool doInitialise()override;
		void doCleanup()override;
		void doFill()override;
		void doUpload()override;

	public:
		static castor::String const Name;

	private:
		//!\~english	The submesh instances count.
		//!\~french		Le nombre d'instances du sous-maillage.
		std::map< MaterialSPtr, uint32_t > m_instanceCount;
		//!\~english	The matrix buffer (instantiation).
		//!\~french		Le tampon de matrices (instanciation).
		VertexBufferUPtr m_matrixBuffer;
	};
}

#endif
