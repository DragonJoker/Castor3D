/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_H___
#define ___C3D_MESH_ANIMATION_H___

#include "Castor3DPrerequisites.hpp"

#include "Animation/Animation.hpp"
#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Submesh animation class.
	\~french
	\brief		Classe d'animation de sous-maillage.
	*/
	class MeshAnimation
		: public Animation
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animable	The parent animable object.
		 *\param[in]	p_name		The name of the animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animable	L'objet animable parent.
		 *\param[in]	p_name		Le nom de l'animation.
		 */
		C3D_API MeshAnimation( Animable & p_animable, castor::String const & p_name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~MeshAnimation();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimation( MeshAnimation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API MeshAnimation & operator=( MeshAnimation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimation( MeshAnimation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API MeshAnimation & operator=( MeshAnimation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	p_object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	p_object	L'enfant.
		 */
		C3D_API void addChild( MeshAnimationSubmesh && p_object );
		/**
		 *\~english
		 *\param[in]	p_index	The index.
		 *\return		The animated submesh at given index.
		 *\~french
		 *\param[in]	p_index	L'indice.
		 *\return		Le sous-maillage animé à l'indice donné.
		 */
		inline MeshAnimationSubmesh & getSubmesh( uint32_t p_index )
		{
			REQUIRE( p_index < m_submeshes.size() );
			return m_submeshes[p_index];
		}

	private:
		/**
		 *\copydoc		Casto3D::Animation::doInitialise
		 */
		C3D_API void doUpdateLength()override;

	protected:
		//!\~english	The animated submesh.
		//!\~french		Les sous-maillages animés.
		MeshAnimationSubmeshArray m_submeshes;

		friend class BinaryWriter< MeshAnimation >;
		friend class BinaryParser< MeshAnimation >;
		friend class MeshAnimationInstance;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for MeshAnimation.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour MeshAnimation.
	*/
	template<>
	struct ChunkTyper< MeshAnimation >
	{
		static ChunkType const Value = ChunkType::eMeshAnimation;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MeshAnimation binary loader.
	\~english
	\brief		Loader binaire de MeshAnimation.
	*/
	template<>
	class BinaryWriter< MeshAnimation >
		: public BinaryWriterBase< MeshAnimation >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( MeshAnimation const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MeshAnimation binary loader.
	\~english
	\brief		Loader binaire de MeshAnimation.
	*/
	template<>
	class BinaryParser< MeshAnimation >
		: public BinaryParserBase< MeshAnimation >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	p_obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	p_obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( MeshAnimation & p_obj )override;
	};
}

#endif
