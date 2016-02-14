/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "BinaryParser.hpp"
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
	class AnimationObjectBase
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
		class BinaryParser
			: public Castor3D::BinaryParser< AnimationObjectBase >
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
			C3D_API virtual bool Fill( AnimationObjectBase const & p_obj, BinaryChunk & p_chunk )const;
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
			C3D_API virtual bool Parse( AnimationObjectBase & p_obj, BinaryChunk & p_chunk )const;
		};

	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		26/01/2016
		\~english
		\brief		Helper structure holding the keyframes and interpolation related stuff.
		\~english
		\brief		Structure d'aide contenant les keyframes et les trucs relatifs à l'interpolation.
		*/
		template< typename T >
		struct KeyFrameInterpolation
		{
			/**
			 *\~english
			 *\brief		Copy assignment operator.
			 *\param[in]	p_rhs	The object to copy.
			 *\return		A reference to this object.
			 *\~french
			 *\brief		Opérateur d'affectation par copie.
			 *\param[in]	p_rhs	L'objet à copier.
			 *\return		Une référence sur cet objet.
			 */
			inline KeyFrameInterpolation< T > & operator=( KeyFrameInterpolation< T > const & p_rhs );
			/**
			 *\~english
			 *\brief		Updates the interpolator.
			 *\param[in]	p_mode	The new mode.
			 *\~french
			 *\brief		Met à jour l'interpolateur.
			 *\param[in]	p_mode	Le nouveau mode.
			 */
			inline void SetInterpolationMode( eINTERPOLATOR_MODE p_mode );
			/**
			 *\~english
			 *\brief		Creates a key frame and adds it to the list.
			 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified.
			 *\param[in]	p_from		The starting time.
			 *\param[in]	p_value		The key frame value.
			 *\param[in]	p_length	The current animation length.
			 *\return		The created key frame.
			 *\~french
			 *\brief		Crée une key frame et l'ajoute à la liste.
			 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée.
			 *\param[in]	p_from		L'index de temps de début.
			 *\param[in]	p_value		La valeur de la key frame.
			 *\param[in]	p_length	La longueur courante de l'animation.
			 *\return		La key frame créée.
			 */
			inline KeyFrame< T > & AddKeyFrame( real p_from, T const & p_value, real & p_length );
			/**
			 *\~english
			 *\brief		Deletes the key frame at time index p_time.
			 *\param[in]	p_time	The time index.
			 *\~french
			 *\brief		Supprime la key frame à l'index de temps donné.
			 *\param[in]	p_time	L'index de temps.
			 */
			inline void RemoveKeyFrame( real p_time );
			/**
			 *\~english
			 *\brief		Computes the animation's value (translation, rotation or scaling) for this object.
			 *\param[in]	p_time	The time index.
			 *\return		The interpolated value.
			 *\~french
			 *\brief		Calcule la valeur (translation, rotation ou mise à l'échelle) pour l'animation de cet objet.
			 *\param[in]	p_time	L'index de temps.
			 *\return		La valeur interpolée.
			 */
			T Compute( real p_time );

			//!\~english The default value when the wanted value can't be interpolated.	\~french La valeur par défaut quand la valeur voulue n'a pas pu être interpolée.
			T m_default;
			//!\~english The key frames sorted by start time.	\~french Les keyframes, triées par index de temps de début.
			KeyFrameArray< T > m_keyframes;
			//!\~english The interpolation mode.	\~french Le mode d'interpolation.
			eINTERPOLATOR_MODE m_mode = eINTERPOLATOR_MODE_NONE;
			//!\~english The interpolator.	\~french L'interpolateur.
			std::unique_ptr< Interpolator< T > > m_interpolator;

			typename KeyFrameArray< T >::const_iterator m_prev = m_keyframes.end();
			typename KeyFrameArray< T >::const_iterator m_curr = m_keyframes.end();
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type	The moving thing type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type	Le type du machin mouvant.
		 */
		C3D_API AnimationObjectBase( eANIMATION_OBJECT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API AnimationObjectBase( AnimationObjectBase const & p_rhs );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AnimationObjectBase();
		/**
		 *\~english
		 *\brief		Adds a child to this object.
		 *\remark		The child's transformations are affected by this object's ones.
		 *\param[in]	p_object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remark		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	p_object	L'enfant.
		 */
		C3D_API void AddChild( AnimationObjectBaseSPtr p_object );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time.
		 *\param[in]	p_time				Current time index.
		 *\param[in]	p_looped			Tells if the animation is looped.
		 *\param[în]	p_transformations	The current transformation matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné.
		 *\param[in]	p_time				Index de temps courant.
		 *\param[in]	p_looped			Dit si l'animation est bouclée.
		 *\param[în]	p_transformations	La matrice de transformation courante.
		 */
		C3D_API void Update( real p_time, bool p_looped, Castor::Matrix4x4r const & p_transformations );
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
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified.
		 *\param[in]	p_from	The starting time.
		 *\param[in]	p_value	The key frame value.
		 *\~french
		 *\brief		Crée une key frame de mise à l'échelle et l'ajoute à la liste.
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée.
		 *\param[in]	p_from	L'index de temps de début.
		 *\param[in]	p_value	La valeur de la key frame.
		 */
		C3D_API Point3rKeyFrame & AddScaleKeyFrame( real p_from, Castor::Point3r const & p_value = Castor::Point3r( 1, 1, 1 ) );
		/**
		 *\~english
		 *\brief		Creates a translation key frame and adds it to the list.
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified.
		 *\param[in]	p_from	The starting time.
		 *\param[in]	p_value	The key frame value.
		 *\~french
		 *\brief		Crée une key frame de translation et l'ajoute à la liste.
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée.
		 *\param[in]	p_from	L'index de temps de début.
		 *\param[in]	p_value	La valeur de la key frame.
		 */
		C3D_API Point3rKeyFrame & AddTranslateKeyFrame( real p_from, Castor::Point3r const & p_value = Castor::Point3r() );
		/**
		 *\~english
		 *\brief		Creates a rotation key frame and adds it to the list.
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified.
		 *\param[in]	p_from	The starting time.
		 *\param[in]	p_value	The key frame value.
		 *\~french
		 *\brief		Crée une key frame de rotation et l'ajoute à la liste.
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée.
		 *\param[in]	p_from	L'index de temps de début.
		 *\param[in]	p_value	La valeur de la key frame.
		 */
		C3D_API QuaternionKeyFrame & AddRotateKeyFrame( real p_from, Castor::Quaternion const & p_value = Castor::Quaternion() );
		/**
		 *\~english
		 *\brief		Deletes the scaling key frame at time index p_time.
		 *\param[in]	p_time	The time index.
		 *\~french
		 *\brief		Supprime la key frame de mise à l'échelle à l'index de temps donné.
		 *\param[in]	p_time	L'index de temps.
		 */
		C3D_API void RemoveScaleKeyFrame( real p_time );
		/**
		 *\~english
		 *\brief		Deletes the translation key frame at time index p_time.
		 *\param[in]	p_time	The time index.
		 *\~french
		 *\brief		Supprime la key frame de translation à l'index de temps donné.
		 *\param[in]	p_time	L'index de temps.
		 */
		C3D_API void RemoveTranslateKeyFrame( real p_time );
		/**
		 *\~english
		 *\brief		Deletes the rotation key frame at time index p_time.
		 *\param[in]	p_time	The time index.
		 *\~french
		 *\brief		Supprime la key frame de rotation à l'index de temps donné.
		 *\param[in]	p_time	L'index de temps.
		 */
		C3D_API void RemoveRotateKeyFrame( real p_time );
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
		C3D_API AnimationObjectBaseSPtr Clone( Animation & p_animation );
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		inline eINTERPOLATOR_MODE GetScaleInterpolationMode()const
		{
			return m_scales.m_mode;
		}
		/**
		 *\~english
		 *\return		The rotation key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de rotation.
		 */
		inline eINTERPOLATOR_MODE GetRotateInterpolationMode()const
		{
			return m_rotates.m_mode;
		}
		/**
		 *\~english
		 *\return		The translation key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de translation.
		 */
		inline eINTERPOLATOR_MODE GetTranslateInterpolationMode()const
		{
			return m_translates.m_mode;
		}
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		inline void SetScaleInterpolationMode( eINTERPOLATOR_MODE p_mode )
		{
			return m_scales.SetInterpolationMode( p_mode );
		}
		/**
		 *\~english
		 *\return		The rotation key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de rotation.
		 */
		inline void SetRotateInterpolationMode( eINTERPOLATOR_MODE p_mode )
		{
			return m_rotates.SetInterpolationMode( p_mode );
		}
		/**
		 *\~english
		 *\return		The translation key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de translation.
		 */
		inline void SetTranslateInterpolationMode( eINTERPOLATOR_MODE p_mode )
		{
			return m_translates.SetInterpolationMode( p_mode );
		}
		/**
		 *\~english
		 *\return		The scale key frames.
		 *\~french
		 *\return		Les key frames de mise à l'échelle.
		 */
		inline Point3rKeyFrameArray const & GetScales()const
		{
			return m_scales.m_keyframes;
		}
		/**
		 *\~english
		 *\return		Translation key frames.
		 *\~french
		 *\return		Les key frames de déplacement.
		 */
		inline Point3rKeyFrameArray const & GetTranslates()const
		{
			return m_translates.m_keyframes;
		}
		/**
		 *\~english
		 *\return		The rotation key frames.
		 *\~french
		 *\return		Les key frames de rotation.
		 */
		inline QuaternionKeyFrameArray const & GetRotates()const
		{
			return m_rotates.m_keyframes;
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
			return !m_rotates.m_keyframes.empty() || !m_scales.m_keyframes.empty() || !m_translates.m_keyframes.empty();
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
		C3D_API virtual AnimationObjectBaseSPtr DoClone( Animation & p_animation ) = 0;

	protected:
		//!\~english The moving thing type.	\~french Le type du machin mouvant.
		eANIMATION_OBJECT_TYPE m_type;
		//!\~english The scaling key frames.	\~french Les keyframes de mise à l'échelle.
		KeyFrameInterpolation< Castor::Point3r > m_scales;
		//!\~english The rotation key frames.	\~french Les keyframes de rotation.
		KeyFrameInterpolation< Castor::Quaternion > m_rotates;
		//!\~english The translation key frames.	\~french Les keyframes de translation.
		KeyFrameInterpolation< Castor::Point3r > m_translates;
		//!\~english Animation node transformations.	\~french Transformations du noeud d'animation.
		Castor::Matrix4x4r m_nodeTransform;
		//!\~english The animation length.	\~french La durée de l'animation.
		real m_length;
		//!\~english The objects depending on this one.	\~french Les objets dépendant de celui-ci.
		AnimationObjectPtrArray m_children;
		//!\~english The cumulative animation transformations.	\~french Les transformations cumulées de l'animation.
		Castor::Matrix4x4r m_cumulativeTransform;
		//!\~english The matrix holding transformation at current time.	\~french La matrice de transformation complète au temps courant de l'animation.
		Castor::Matrix4x4r m_finalTransform;
	};
}

#include "AnimationObjectBase.inl"

#endif
