/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_H___
#define ___C3D_SKELETON_ANIMATION_H___

#include "Castor3DPrerequisites.hpp"

#include "Animation/Animation.hpp"
#include "SkeletonAnimationObject.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Skeleton animation class.
	\~french
	\brief		Classe d'animation de squelette.
	*/
	class SkeletonAnimation
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
		C3D_API SkeletonAnimation( Animable & p_animable, castor::String const & p_name = castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimation();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimation( SkeletonAnimation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimation & operator=( SkeletonAnimation && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimation( SkeletonAnimation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimation & operator=( SkeletonAnimation const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Creates and adds a moving node.
		 *\param[in]	p_name		The node name.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un noeud mouvant.
		 *\param[in]	p_name		Le nom du noeud.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( castor::String const & p_name, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Creates and adds a moving bone.
		 *\param[in]	p_bone		The bone to add.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Crée et ajoute un os mouvant.
		 *\param[in]	p_bone		L'os.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( BoneSPtr p_bone, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		adds an animated object.
		 *\param[in]	p_object	The animated object to add.
		 *\param[in]	p_parent	The moving object's parent.
		 *\~french
		 *\brief		Ajoute un objet animé.
		 *\param[in]	p_object	L'objet animé.
		 *\param[in]	p_parent	Le parent de l'objet déplaçable.
		 */
		C3D_API SkeletonAnimationObjectSPtr addObject( SkeletonAnimationObjectSPtr p_object, SkeletonAnimationObjectSPtr p_parent );
		/**
		 *\~english
		 *\brief		Tells if the animation has the animated object.
		 *\param[in]	p_type	The object type.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Dit si l'animation a l'objet animé.
		 *\param[in]	p_type	Le type de l'objet.
		 *\param[in]	p_name	Le nom de l'objet.
		 */
		C3D_API bool hasObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated bone.
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Récupère un os animé.
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( Bone const & p_bone )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	p_name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object.
		 *\param[in]	p_type	The object type.
		 *\param[in]	p_name	The object name.
		 *\~french
		 *\brief		Récupère un objet animé.
		 *\param[in]	p_type	Le type d'objet.
		 *\param[in]	p_name	Le nom de l'objet.
		 */
		C3D_API SkeletonAnimationObjectSPtr getObject( SkeletonAnimationObjectType p_type, castor::String const & p_name )const;
		/**
		 *\~english
		 *\return		The moving objects.
		 *\~french
		 *\return		Les objets mouvants.
		 */
		inline SkeletonAnimationObjectPtrStrMap const & getObjects()const
		{
			return m_toMove;
		}
		/**
		 *\~english
		 *\return		The root moving objects.
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Les objets mouvants racines.
		 */
		inline SkeletonAnimationObjectPtrArray const & getRootObjects()const
		{
			return m_arrayMoving;
		}

	private:
		/**
		 *\copydoc		Casto3D::Animation::doUpdateLength
		 */
		C3D_API void doUpdateLength()override;

	protected:
		//!\~english	The root moving objects.
		//!\~french		Les objets mouvants racine.
		SkeletonAnimationObjectPtrArray m_arrayMoving;
		//!\~english	The moving objects.
		//!\~french		Les objets mouvants.
		SkeletonAnimationObjectPtrStrMap m_toMove;

		friend class BinaryWriter< SkeletonAnimation >;
		friend class BinaryParser< SkeletonAnimation >;
		friend class SkeletonAnimationInstance;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimation.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimation.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimation >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimation;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimation binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimation.
	*/
	template<>
	class BinaryWriter< SkeletonAnimation >
		: public BinaryWriterBase< SkeletonAnimation >
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
		C3D_API bool doWrite( SkeletonAnimation const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimation binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimation.
	*/
	template<>
	class BinaryParser< SkeletonAnimation >
		: public BinaryParserBase< SkeletonAnimation >
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
		C3D_API bool doParse( SkeletonAnimation & p_obj )override;
	};
}

#endif
