/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TRANSFORM_FEEDBACK_H___
#define ___C3D_TRANSFORM_FEEDBACK_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

#include <cstddef>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief		Transform feedback implementation.
	\~french
	\brief		Implémentation du Transform feedback.
	*/
	class TransformFeedback
		: castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderSystem	The RenderSystem
		 *\param[in]	p_computed		The computed elements description.
		 *\param[in]	p_topology		The primitives type.
		 *\param[in]	p_program		The shader program.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	p_computed		La description des éléments calculés.
		 *\param[in]	p_topology		Le type de primitives.
		 *\param[in]	p_program		Le programm shader.
		 */
		C3D_API TransformFeedback( RenderSystem & renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TransformFeedback();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	p_buffers	The vertex buffers.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	p_buffers	Les tampons de sommets.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( renderer::VertexBufferCRefArray const & p_buffers );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		C3D_API void bind()const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API void unbind()const;
		/**
		 *\~english
		 *\return		The primitives written at last call.
		 *\~french
		 *\brief		Les primitives écrites au dernier appel.
		 */
		inline uint32_t getWrittenPrimitives()const
		{
			return m_writtenPrimitives;
		}

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		C3D_API virtual void doBind()const = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 */
		C3D_API virtual void doUnbind()const = 0;
		/**
		 *\~english
		 *\brief		Begins transform feedback.
		 *\~french
		 *\brief		Démarre le transform feedback.
		 */
		C3D_API virtual void doBegin()const = 0;
		/**
		 *\~english
		 *\brief		Ends transform feedback.
		 *\~french
		 *\brief		Termine le transform feedback.
		 */
		C3D_API virtual void doEnd()const = 0;

	protected:
		//!\~english	The shader program.
		//!\~french		Le programm shader.
		ShaderProgram & m_program;
		//!\~english	The computed elements description.
		//!\~french		La description des éléments calculés.
		BufferDeclaration m_computed;
		//!\~english	The attached buffers.
		//!\~french		Les tampons attachés.
		renderer::VertexBufferCRefArray m_buffers;
		//!\~english	The primitives written GPU query.
		//!\~french		La requête GPU de primitives écrites.
		GpuQueryUPtr m_query;
		//!\~english	The written primitives count.
		//!\~french		Le nomnbre de primitives écrites.
		mutable uint32_t m_writtenPrimitives{ 1u };
		//!\~english	Tells if the next call to Draw is the first one.
		//!\~french		Dit si le prochain appel à Draw est le premier.
		bool m_firstDraw{ true };
	};
}

#endif
