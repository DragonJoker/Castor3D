/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BonesInstantiationComponent_H___
#define ___C3D_BonesInstantiationComponent_H___

#include "BonesComponent.hpp"
#include "InstantiationComponent.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		The submesh component used for instantiated submeshes with bones.
	\~french
	\brief		Le composant de sous-maillage utilisé pour les sous-maillages avec des bones et instanciés.
	*/
	class BonesInstantiationComponent
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
		C3D_API BonesInstantiationComponent( Submesh & submesh
			, InstantiationComponent const & instantiation
			, BonesComponent const & bones );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~BonesInstantiationComponent();
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		inline SkeletonSPtr getSkeleton()const
		{
			return m_bones.getSkeleton();
		}
		/**
		 *\~english
		 *\return		Tells if the bone instantiation buffer exists.
		 *\~french
		 *\return		Dit si le tampon d'instanciation des os existe.
		 */
		inline bool hasInstancedBonesBuffer()const
		{
			return bool( m_instancedBonesBuffer );
		}
		/**
		 *\~english
		 *\return		The bone instantiation ShaderStorageBuffer.
		 *\~french
		 *\return		Le ShaderStorageBuffer d'instanciation des os.
		 */
		inline ShaderStorageBuffer const & getInstancedBonesBuffer()const
		{
			return *m_instancedBonesBuffer;
		}
		/**
		 *\~english
		 *\return		The bone instantiation ShaderStorageBuffer.
		 *\~french
		 *\return		Le ShaderStorageBuffer d'instanciation des os.
		 */
		inline ShaderStorageBuffer & getInstancedBonesBuffer()
		{
			return *m_instancedBonesBuffer;
		}
		/**
		 *\~english
		 *\return		The shader program flags.
		 *\~french
		 *\return		Les indicateurs de shader.
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return ProgramFlags( 0 );
		}

	private:
		bool doInitialise()override;
		void doCleanup()override;
		void doFill()override;
		void doUpload()override;

	public:
		static castor::String const Name;

	private:
		//!\~english	The instantiation component.
		//!\~french		Le composant d'instantiation.
		InstantiationComponent const & m_instantiation;
		//!\~english	The skinning component.
		//!\~french		Le composant de skinning.
		BonesComponent const & m_bones;
		//!\~english	The bones buffer (instantiation).
		//!\~french		Le tampon de bones (instanciation).
		ShaderStorageBufferSPtr m_instancedBonesBuffer;
	};
}

#endif
