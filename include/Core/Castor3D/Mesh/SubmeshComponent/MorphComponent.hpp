/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphComponent_H___
#define ___C3D_MorphComponent_H___

#include "Castor3D/Mesh/SubmeshComponent/SubmeshComponent.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		The submesh component used for morphing.
	\~french
	\brief		Le composant de sous-maillage pour le morphing.
	*/
	class MorphComponent
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
		C3D_API explicit MorphComponent( Submesh & submesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MorphComponent();
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( MaterialSPtr material
			, ashes::BufferCRefArray & buffers
			, std::vector< uint64_t > & offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )override;
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline ashes::VertexBuffer< InterleavedVertex > const & getAnimationBuffer()const
		{
			return *m_animBuffer;
		}
		/**
		 *\~english
		 *\return		The VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer.
		 */
		inline ashes::VertexBuffer< InterleavedVertex > & getAnimationBuffer()
		{
			return *m_animBuffer;
		}
		/**
		 *\~english
		 *\return		The animated vertex buffer data.
		 *\~french
		 *\return		Les données du tampon de sommets animés.
		 */
		inline InterleavedVertexArray & getData()
		{
			return m_data;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags( MaterialSPtr material )const override
		{
			return ProgramFlag::eMorphing;
		}

	private:
		bool doInitialise()override;
		void doCleanup()override;
		void doFill()override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;
		C3D_API static uint32_t constexpr BindingPoint = 1u;

	private:
		ashes::VertexBufferPtr< InterleavedVertex > m_animBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_animLayout;
		InterleavedVertexArray m_data;
	};
}

#endif
