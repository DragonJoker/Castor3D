/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_ANIMATION_OBJECT_BASE_H___
#define ___C3D_ANIMATION_OBJECT_BASE_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Interpolator.hpp"

#include <SquareMatrix.hpp>
#include <Quaternion.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things.
	\remark		Manages translation, scaling, rotation of the thing.
	\~french
	\brief		Classe de représentation de choses mouvantes.
	\remark		Gère les translations, mises à l'échelle, rotations de la chose.
	*/
	class AnimationObject
		: public Castor::OwnedBy< Animation >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		MovingObjectBase binary loader.
		\~english
		\brief		Loader binaire de MovingObjectBase.
		*/
		class BinaryWriter
			: public Castor3D::BinaryWriter< AnimationObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_path	The current folder path.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_path	Le chemin d'accès au dossier courant.
			 */
			C3D_API BinaryWriter( Castor::Path const & p_path );

		private:
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data.
			 *\param[in]	p_obj	The object to write.
			 *\param[out]	p_chunk	The chunk to fill.
			 *\return		\p false if any error occured.
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
			 *\param[in]	p_obj	L'objet à écrire.
			 *\param[out]	p_chunk	Le chunk à remplir.
			 *\return		\p false si une erreur quelconque est arrivée.
			 */
			C3D_API bool DoWrite( AnimationObject const & p_obj, BinaryChunk & p_chunk )const override;
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		MovingObjectBase binary loader.
		\~english
		\brief		Loader binaire de MovingObjectBase.
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< AnimationObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_path	The current folder path.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_path	Le chemin d'accès au dossier courant.
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );

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
			C3D_API bool DoParse( AnimationObject & p_obj, BinaryChunk & p_chunk )const override;
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\param[in]	p_type		The moving thing type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 *\param[in]	p_type		Le type du machin mouvant.
		 */
		C3D_API AnimationObject( Animation & p_animation, eANIMATION_OBJECT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimationObject( AnimationObject const & p_rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AnimationObject();
		/**
		 *\~english
		 *\brief		Adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	p_object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	p_object	L'enfant.
		 */
		C3D_API void AddChild( AnimationObjectSPtr p_object );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time.
		 *\param[in]	p_time				Current time index.
		 *\param[in]	p_transformations	The current transformation matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné.
		 *\param[in]	p_time				Index de temps courant.
		 *\param[in]	p_transformations	La matrice de transformation courante.
		 */
		C3D_API void Update( real p_time, Castor::Matrix4x4r const & p_transformations );
		/**
		 *\~english
		 *\brief		Retrieves the object name.
		 *\return		The name.
		 *\~french
		 *\brief		Récupère le nom de l'objet.
		 *\return		Le nom.
		 */
		C3D_API virtual Castor::String const & GetName()const = 0;
		/**
		 *\~english
		 *\brief		Creates a scaling key frame and adds it to the list.
		 *\remarks		If a key frame with the same starting time already exists, it is returned, but not modified.
		 *\param[in]	p_from		The starting time.
		 *\param[in]	p_translate	The translation at start time.
		 *\param[in]	p_rotate	The rotation at start time.
		 *\param[in]	p_scale		The scaling at start time.
		 *\~french
		 *\brief		Crée une key frame de mise à l'échelle et l'ajoute à la liste.
		 *\remarks		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée.
		 *\param[in]	p_from		L'index de temps de début.
		 *\param[in]	p_translate	La translation au temps de début.
		 *\param[in]	p_rotate	La rotation au temps de début.
		 *\param[in]	p_scale		L'échelle au temps de début.
		 */
		C3D_API KeyFrame & AddKeyFrame( real p_from, Castor::Point3r const & p_translate = Castor::Point3r{}, Castor::Quaternion const & p_rotate = Castor::Quaternion{}, Castor::Point3r const & p_scale = Castor::Point3r{ 1.0_r, 1.0_r, 1.0_r } );
		/**
		 *\~english
		 *\brief		Deletes the scaling key frame at time index p_time.
		 *\param[in]	p_time	The time index.
		 *\~french
		 *\brief		Supprime la key frame de mise à l'échelle à l'index de temps donné.
		 *\param[in]	p_time	L'index de temps.
		 */
		C3D_API void RemoveKeyFrame( real p_time );
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		C3D_API void SetInterpolationMode( eINTERPOLATOR_MODE p_mode );
		/**
		 *\~english
		 *\brief		Clones this moving thing.
		 *\param[out]	p_animation	The clone is added to this animation.
		 *\return		The clone.
		 *\~french
		 *\brief		Clône cet objet animable.
		 *\param[out]	p_animation	Le clône est ajouté à cette animation.
		 *\return		Le clône.
		 */
		C3D_API AnimationObjectSPtr Clone( Animation & p_animation );
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		inline eINTERPOLATOR_MODE GetInterpolationMode()const
		{
			return m_mode;
		}
		/**
		 *\~english
		 *\return		The key frames.
		 *\~french
		 *\return		Les key frames.
		 */
		inline KeyFrameArray const & GetKeyFrames()const
		{
			return m_keyframes;
		}
		/**
		 *\~english
		 *\return		The animation length.
		 *\~french
		 *\return		La durée de l'animation.
		 */
		inline real GetLength()const
		{
			return m_length;
		}
		/**
		 *\~english
		 *\return		The moving object type.
		 *\~french
		 *\return		Le type d'objet mouvant.
		 */
		inline eANIMATION_OBJECT_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the animation length.
		 *\param[in]	p_length	The new value.
		 *\~french
		 *\brief		Définit la durée de l'animation.
		 *\param[in]	p_length	La nouvelle valeur.
		 */
		inline void	SetLength( real p_length )
		{
			m_length = p_length;
		}
		/**
		 *\~english
		 *\brief		The final object's animations transformation.
		 *\~french
		 *\brief		La transfomation finale des animations du de cet objet.
		 */
		inline Castor::Matrix4x4r const & GetFinalTransform()const
		{
			return m_finalTransform;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère les animations du noeud de transformation.
		 *\return		La valeur.
		 */
		inline Castor::Matrix4x4r const & GetNodeTransform()const
		{
			return m_nodeTransform;
		}
		/**
		 *\~english
		 *\brief		Sets the animation node transformation.
		 *\param[in]	p_transform	The new value.
		 *\~french
		 *\brief		Définit les animations du noeud de transformation.
		 *\param[in]	p_transform	La nouvelle valeur.
		 */
		inline void SetNodeTransform( Castor::Matrix4x4r const & p_transform )
		{
			m_nodeTransform = p_transform;
		}
		/**
		 *\~english
		 *\brief		Tells whether or not the object has keyframes.
		 *\return		\p false if no keyframes.
		 *\~french
		 *\brief		Dit si l'objet a des keyframes.
		 *\return		\p false si pas de keyframes.
		 */
		inline bool HasKeyFrames()const
		{
			return !m_keyframes.empty();
		}
		/**
		 *\~english
		 *\return		The children array.
		 *\~french
		 *\return		Le tableau d'enfants.
		 */
		inline AnimationObjectPtrArray const & GetChildren()const
		{
			return m_children;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet.
		 */
		C3D_API virtual void DoApply() = 0;
		/**
		 *\~english
		 *\brief		Clones this moving thing.
		 *\param[out]	p_animation	The clone is added to this animation.
		 *\return		The clone.
		 *\~french
		 *\brief		Clône cet objet animable.
		 *\param[out]	p_animation	Le clône est ajouté à cette animation.
		 *\return		Le clône.
		 */
		C3D_API virtual AnimationObjectSPtr DoClone( Animation & p_animation ) = 0;

	protected:
		//!\~english The interpolation mode.	\~french Le mode d'interpolation.
		eINTERPOLATOR_MODE m_mode{ eINTERPOLATOR_MODE_COUNT };
		//!\~english The animation length.	\~french La durée de l'animation.
		real m_length{ 0.0_r };
		//!\~english The moving thing type.	\~french Le type du machin mouvant.
		eANIMATION_OBJECT_TYPE m_type;
		//!\~english The point interpolator.	\~french L'interpolateur de points.
		std::unique_ptr< Point3rInterpolator > m_pointInterpolator;
		//!\~english The quaternion interpolator.	\~french L'interpolateur de quaternions.
		std::unique_ptr< QuaternionInterpolator > m_quaternionInterpolator;
		//!\~english The key frames.	\~french Les keyframes.
		KeyFrameArray m_keyframes;
		//!\~english Iterator to the previous keyframe (when playing the animation).	\~french Itérateur sur la key frame précédente (quand l'animation est jouée).
		KeyFrameArray::const_iterator m_prev;
		//!\~english Iterator to the current keyframe (when playing the animation).	\~french Itérateur sur la key frame courante (quand l'animation est jouée).
		KeyFrameArray::const_iterator m_curr;
		//!\~english Animation node transformations.	\~french Transformations du noeud d'animation.
		Castor::Matrix4x4r m_nodeTransform;
		//!\~english The objects depending on this one.	\~french Les objets dépendant de celui-ci.
		AnimationObjectPtrArray m_children;
		//!\~english The cumulative animation transformations.	\~french Les transformations cumulées de l'animation.
		Castor::Matrix4x4r m_cumulativeTransform;
		//!\~english The matrix holding transformation at current time.	\~french La matrice de transformation complète au temps courant de l'animation.
		Castor::Matrix4x4r m_finalTransform;
	};
}

#endif
