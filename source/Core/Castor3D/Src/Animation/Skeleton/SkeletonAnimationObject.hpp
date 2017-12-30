/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SKELETON_ANIMATION_OBJECT_H___
#define ___C3D_SKELETON_ANIMATION_OBJECT_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Animation/Interpolator.hpp"

#include <Graphics/BoundingBox.hpp>
#include <Math/SquareMatrix.hpp>
#include <Math/Quaternion.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the skeleton animation objects.
	\remark		Manages translation, scaling, rotation of the object.
	\~french
	\brief		Classe de représentation d'objets d'animations de squelette.
	\remark		Gère les translations, mises à l'échelle, rotations de l'objet.
	*/
	class SkeletonAnimationObject
		: public castor::OwnedBy< SkeletonAnimation >
		, public std::enable_shared_from_this< SkeletonAnimationObject >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\param[in]	type		The skeleton animation object type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 *\param[in]	type		Le type d'objet d'animation de squelette.
		 */
		C3D_API SkeletonAnimationObject( SkeletonAnimation & animation
			, SkeletonAnimationObjectType type );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationObject( SkeletonAnimationObject const & rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SkeletonAnimationObject();
		/**
		 *\~english
		 *\brief		adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	object	L'enfant.
		 */
		C3D_API void addChild( SkeletonAnimationObjectSPtr object );
		/**
		 *\~english
		 *\brief		Retrieves the object name.
		 *\return		The name.
		 *\~french
		 *\brief		Récupère le nom de l'objet.
		 *\return		Le nom.
		 */
		C3D_API virtual castor::String const & getName()const = 0;
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		inline InterpolatorType getInterpolationMode()const
		{
			return m_mode;
		}
		/**
		 *\~english
		 *\return		The moving object type.
		 *\~french
		 *\return		Le type d'objet mouvant.
		 */
		inline SkeletonAnimationObjectType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère les animations du noeud de transformation.
		 *\return		La valeur.
		 */
		inline castor::Matrix4x4r const & getNodeTransform()const
		{
			return m_nodeTransform;
		}
		/**
		 *\~english
		 *\brief		sets the animation node transformation.
		 *\param[in]	transform	The new value.
		 *\~french
		 *\brief		Définit les animations du noeud de transformation.
		 *\param[in]	transform	La nouvelle valeur.
		 */
		inline void setNodeTransform( castor::Matrix4x4r const & transform )
		{
			m_nodeTransform = transform;
		}
		/**
		 *\~english
		 *\return		The children array.
		 *\~french
		 *\return		Le tableau d'enfants.
		 */
		inline SkeletonAnimationObjectPtrArray const & getChildren()const
		{
			return m_children;
		}
		/**
		 *\~english
		 *\return		The parent object.
		 *\~french
		 *\return		L'objet parent.
		 */
		inline SkeletonAnimationObjectSPtr getParent()const
		{
			return m_parent.lock();
		}

	protected:
		//!\~english	The interpolation mode.
		//!\~french		Le mode d'interpolation.
		InterpolatorType m_mode{ InterpolatorType::eCount };
		//!\~english	The moving thing type.
		//!\~french		Le type du machin mouvant.
		SkeletonAnimationObjectType m_type;
		//!\~english	Animation node transformations.
		//!\~french		Transformations du noeud d'animation.
		castor::Matrix4x4r m_nodeTransform;
		//!\~english	The objects depending on this one.
		//!\~french		Les objets dépendant de celui-ci.
		SkeletonAnimationObjectPtrArray m_children;
		//!\~english	The parent object.
		//!\~french		L'objet parent.
		SkeletonAnimationObjectWPtr m_parent;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;

		friend class BinaryWriter< SkeletonAnimationObject >;
		friend class BinaryParser< SkeletonAnimationObject >;
		friend class SkeletonAnimationInstanceObject;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationObject.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationObject.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationObject >
	{
		static ChunkType const Value = ChunkType::eAnimationObject;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimationObject binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationObject.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationObject >
		: public BinaryWriterBase< SkeletonAnimationObject >
	{
	protected:
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
		C3D_API bool doWrite( SkeletonAnimationObject const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimationObject binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationObject.
	*/
	template<>
	class BinaryParser< SkeletonAnimationObject >
		: public BinaryParserBase< SkeletonAnimationObject >
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
		C3D_API bool doParse( SkeletonAnimationObject & obj )override;
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk in version 1.1.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk en version 1.1.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse_v1_1( SkeletonAnimationObject & obj )override;
	};
}

#endif
