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
#ifndef ___C3D_MOVING_OBJECT_BASE_H___
#define ___C3D_MOVING_OBJECT_BASE_H___

#include "Castor3DPrerequisites.hpp"

#include <SquareMatrix.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things
	\remark		Manages translation, scaling, rotation of the thing
	\~french
	\brief		Classe de représentation de choses mouvantes
	\remark		Gère les translations, mises à l'échelle, rotations de la chose
	*/
	typedef enum eMOVING_OBJECT_TYPE CASTOR_TYPE( uint8_t )
	{
		eMOVING_OBJECT_TYPE_NODE,
		eMOVING_OBJECT_TYPE_OBJECT,
		eMOVING_OBJECT_TYPE_BONE,
	}	eMOVING_OBJECT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things
	\remark		Manages translation, scaling, rotation of the thing
	\~french
	\brief		Classe de représentation de choses mouvantes
	\remark		Gère les translations, mises à l'échelle, rotations de la chose
	*/
	class MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type	The moving thing type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type	Le type du machin mouvant
		 */
		C3D_API MovingObjectBase( eMOVING_OBJECT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MovingObjectBase();
		/**
		 *\~english
		 *\brief		Adds a child to this object
		 *\remark		The child's transformations are affected by this object's ones
		 *\param[in]	p_object	The child
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci
		 *\remark		Les transformations de l'enfant sont affectées par celles de cet objet
		 *\param[in]	p_object	L'enfant
		 */
		C3D_API void AddChild( MovingObjectBaseSPtr p_object );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time
		 *\param[in]	p_time				Current time index
		 *\param[in]	p_looped			Tells if the animation is looped
		 *\param[în]	p_transformations	The current transformation matrix
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné
		 *\param[in]	p_time				Index de temps courant
		 *\param[in]	p_looped			Dit si l'animation est bouclée
		 *\param[în]	p_transformations	La matrice de transformation courante
		 */
		C3D_API void Update( real p_time, bool p_looped, Castor::Matrix4x4r const & p_transformations );
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		C3D_API virtual Castor::String const & GetName()const = 0;
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_from	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_from	L'index de temps de début
		 */
		C3D_API ScaleKeyFrameSPtr AddScaleKeyFrame( real p_from );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_from	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_from	L'index de temps de début
		 */
		C3D_API TranslateKeyFrameSPtr AddTranslateKeyFrame( real p_from );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_from	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_from	L'index de temps de début
		 */
		C3D_API RotateKeyFrameSPtr AddRotateKeyFrame( real p_from );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_time	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
		 *\param[in]	p_time	L'index de temps
		 */
		C3D_API void RemoveScaleKeyFrame( real p_time );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_time	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
		 *\param[in]	p_time	L'index de temps
		 */
		C3D_API void RemoveTranslateKeyFrame( real p_time );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_time	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
		 *\param[in]	p_time	L'index de temps
		 */
		C3D_API void RemoveRotateKeyFrame( real p_time );
		/**
		 *\~english
		 *\brief		Clones this moving thing
		 *\param[out]	p_map	Receives the created clone and it's childs
		 *\return		The clone
		 *\~french
		 *\brief		Clone cet objet animable
		 *\param[out]	p_map	Reçoit les enfants clonés en plus de l'objet cloné
		 *\return		Le clone
		 */
		C3D_API MovingObjectBaseSPtr Clone( MovingObjectPtrStrMap & p_map );
		/**
		 *\~english
		 *\return		The scale key frames.
		 *\~french
		 *\return		Les key frames de mise à l'échelle.
		 */
		inline ScaleKeyFramePtrRealMap const & GetScales()const
		{
			return m_scales;
		}
		/**
		 *\~english
		 *\return		Translation key frames.
		 *\~french
		 *\return		Les key frames de déplacement.
		 */
		inline TranslateKeyFramePtrRealMap const & GetTranslates()const
		{
			return m_translates;
		}
		/**
		 *\~english
		 *\return		The rotation key frames.
		 *\~french
		 *\return		Les key frames de rotation.
		 */
		inline RotateKeyFramePtrRealMap const & GetRotates()const
		{
			return m_rotates;
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
		 *\brief		Sets the animation length
		 *\param[in]	p_length	The new value
		 *\~french
		 *\brief		Définit la durée de l'animation
		 *\param[in]	p_length	La nouvelle valeur
		 */
		inline void	SetLength( real p_length )
		{
			m_length = p_length;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation
		 *\return		The value
		 *\~french
		 *\brief		Récupère les animations du noeud de transformation
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetNodeTransform()const
		{
			return m_nodeTransform;
		}
		/**
		 *\~english
		 *\brief		Sets the animation node transformation
		 *\param[in]	p_mtxTransform	The new value
		 *\~french
		 *\brief		Définit les animations du noeud de transformation
		 *\param[in]	p_mtxTransform	La nouvelle valeur
		 */
		inline void SetNodeTransform( Castor::Matrix4x4r const & p_mtxTransform )
		{
			m_nodeTransform = p_mtxTransform;
		}
		/**
		 *\~english
		 *\brief		Tells whether or not the object has keyframes
		 *\return		\p false if no keyframes
		 *\~french
		 *\brief		Dit si l'objet a des keyframes
		 *\return		\p false si pas de keyframes
		 */
		inline bool HasKeyFrames()const
		{
			return !m_rotates.empty() || !m_scales.empty() || !m_translates.empty();
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving thuing type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type du machin mouvant
		 *\return		La valeur
		 */
		inline eMOVING_OBJECT_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves final transformation matrix for current animation time
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\return		La valeur
		 */
		inline const Castor::Matrix4x4r & GetFinalTransformation()const
		{
			return m_finalTransformation;
		}
		/**
		 *\~english
		 *\brief		Sets final transformation for current animation time
		 *\param[in]	p_mtx	The new value
		 *\~french
		 *\brief		Définit la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\param[in]	p_mtx	La nouvelle valeur
		 */
		inline void SetFinalTransformation( const Castor::Matrix4x4r & p_mtx )
		{
			m_finalTransformation = p_mtx;
		}
		/**
		 *\~english
		 *\return		The children.
		 *\~french
		 *\return		Les enfants.
		 */
		inline MovingObjectPtrArray const & GetChildren()const
		{
			return m_children;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet
		 */
		C3D_API virtual void DoApply() = 0;
		/**
		 *\~english
		 *\brief		Clones this moving thing
		 *\return		The clone
		 *\~french
		 *\brief		Clone cet objet animable
		 *\return		Le clone
		 */
		C3D_API virtual MovingObjectBaseSPtr DoClone() = 0;

	private:
		Castor::Matrix4x4r DoComputeTransform( real p_time );
		Castor::Point3r DoComputeScaling( real p_time );
		Castor::Point3r DoComputeTranslation( real p_time );
		Castor::Quaternion DoComputeRotation( real p_time );
		template< class KeyFrameType >
		std::shared_ptr< KeyFrameType > DoAddKeyFrame( real p_from, std::map< real, std::shared_ptr< KeyFrameType > > & p_map );
		template< class KeyFrameType >
		void DoRemoveKeyFrame( real p_time, std::map< real, std::shared_ptr< KeyFrameType > > & p_map );
		template< eINTERPOLATOR_MODE Mode, class ValueType, class KeyFrameType >
		ValueType DoCompute( real p_time, std::map< real, std::shared_ptr< KeyFrameType > > const & p_map, ValueType const & p_default );

	protected:
		//!\~english The key frames sorted by start time	\~french Les keyframes, triées par index de temps de début
		ScaleKeyFramePtrRealMap m_scales;
		//!\~english The key frames sorted by start time	\~french Les keyframes, triées par index de temps de début
		TranslateKeyFramePtrRealMap m_translates;
		//!\~english The key frames sorted by start time	\~french Les keyframes, triées par index de temps de début
		RotateKeyFramePtrRealMap m_rotates;
		//!\~english The animation length	\~french La durée de l'animation
		real m_length;
		//!\~english The objects depending on this one	\~french Les objets dépendant de celui-ci
		MovingObjectPtrArray m_children;
		//!\~english Animation node transformations	\~french Transformations du noeud d'animation
		Castor::Matrix4x4r m_nodeTransform;
		//!\~english Actual transformations	\~french Transformations actuelles
		Castor::Matrix4x4r m_transformations;
		//!\~english The moving thing type	\~french Le type du machin mouvant
		eMOVING_OBJECT_TYPE m_type;
		//!\~english The matrix holding transformation at current time	\~french La matrice de transformation complète au temps courant de l'animation
		Castor::Matrix4x4r m_finalTransformation;
	};
}

#include "MovingObjectBase.inl"

#endif


