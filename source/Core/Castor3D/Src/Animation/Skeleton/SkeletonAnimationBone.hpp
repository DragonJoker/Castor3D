/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_BONE___
#define ___C3D_SKELETON_ANIMATION_BONE___

#include "SkeletonAnimationObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationBone
		: public SkeletonAnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 */
		C3D_API explicit SkeletonAnimationBone( SkeletonAnimation & animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimationBone();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone && rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone const & rhs ) = delete;
		/**
		 *\~english
		 *\return		The object name.
		 *\~french
		 *\return		Le nom de l'objet.
		 */
		virtual castor::String const & getName()const;
		/**
		 *\~english
		 *\brief		Sets the moving object.
		 *\param[in]	bone		The bone.
		 *\~french
		 *\brief		Définit l'objet mouvant.
		 *\param[in]	bone		L'os.
		 */
		inline void setBone( BoneSPtr bone )
		{
			m_bone = bone;
		}
		/**
		 *\~english
		 *\return		The moving object.
		 *\~french
		 *\return		L'objet mouvant.
		 */
		inline BoneSPtr getBone()const
		{
			return m_bone.lock();
		}

	private:
		//!\~english	The bone affected by the animations.
		//!\~french		L'os affecté par les animations.
		BoneWPtr m_bone;

		friend class BinaryWriter< SkeletonAnimationBone >;
		friend class BinaryParser< SkeletonAnimationBone >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationBone.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationBone.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationBone >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimationBone;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationBone >
		: public BinaryWriterBase< SkeletonAnimationBone >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( SkeletonAnimationBone const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryParser< SkeletonAnimationBone >
		: public BinaryParserBase< SkeletonAnimationBone >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( SkeletonAnimationBone & obj )override;
	};
}

#endif
