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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	\brief		Classe de repr�sentation de choses mouvantes
	\remark		G�re les translations, mises � l'�chelle, rotations de la chose
	*/
	typedef enum eMOVING_OBJECT_TYPE CASTOR_TYPE( uint8_t )
	{
		eMOVING_OBJECT_TYPE_NODE
		,	eMOVING_OBJECT_TYPE_OBJECT
		,	eMOVING_OBJECT_TYPE_BONE
	}	eMOVING_OBJECT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things
	\remark		Manages translation, scaling, rotation of the thing
	\~french
	\brief		Classe de repr�sentation de choses mouvantes
	\remark		G�re les translations, mises � l'�chelle, rotations de la chose
	*/
	class C3D_API MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType	The moving thing type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType	Le type du machin mouvant
		 */
		MovingObjectBase( eMOVING_OBJECT_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingObjectBase();
		/**
		 *\~english
		 *\brief		Adds a child to this object
		 *\remark		The child's transformations are affected by this object's ones
		 *\param[in]	p_Object	The child
		 *\~french
		 *\brief		Ajoute un objet enfant � celui-ci
		 *\remark		Les transformations de l'enfant sont affect�es par celles de cet objet
		 *\param[in]	p_pObject	L'enfant
		 */
		void AddChild( MovingObjectBaseSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time
		 *\param[in]	p_rTime					Current time index
		 *\param[in]	p_bLooped				Tells if the animation is looped
		 *\param[�n]	p_mtxTransformations	The current transformation matrix
		 *\~french
		 *\brief		Met � jour les transformations appliqu�es � l'objet, l'index de temps donn�
		 *\param[in]	p_rTime					Index de temps courant
		 *\param[in]	p_bLooped				Dit si l'animation est boucl�e
		 *\param[�n]	p_mtxTransformations	La matrice de transformation courante
		 */
		void Update( real p_rTime, bool p_bLooped, Castor::Matrix4x4r const & p_mtxTransformations );
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		R�cup�re le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const = 0;
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Cr�e une key frame et l'ajoute � la liste
		 *\remark		Si une key frame avec le m�me index de temps de d�but existe, elle est retourn�e sans �tre modifi�e
		 *\param[in]	p_rFrom	L'index de temps de d�but
		 */
		ScaleKeyFrameSPtr AddScaleKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Cr�e une key frame et l'ajoute � la liste
		 *\remark		Si une key frame avec le m�me index de temps de d�but existe, elle est retourn�e sans �tre modifi�e
		 *\param[in]	p_rFrom	L'index de temps de d�but
		 */
		TranslateKeyFrameSPtr AddTranslateKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Cr�e une key frame et l'ajoute � la liste
		 *\remark		Si une key frame avec le m�me index de temps de d�but existe, elle est retourn�e sans �tre modifi�e
		 *\param[in]	p_rFrom	L'index de temps de d�but
		 */
		RotateKeyFrameSPtr AddRotateKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame � l'index de temps donn�
		 *\param[in]	p_rTime	L'index de temps
		 */
		void RemoveScaleKeyFrame( real p_rTime );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame � l'index de temps donn�
		 *\param[in]	p_rTime	L'index de temps
		 */
		void RemoveTranslateKeyFrame( real p_rTime );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame � l'index de temps donn�
		 *\param[in]	p_rTime	L'index de temps
		 */
		void RemoveRotateKeyFrame( real p_rTime );
		/**
		 *\~english
		 *\brief		Clones this moving thing
		 *\param[out]	p_map	Receives the created clone and it's childs
		 *\return		The clone
		 *\~french
		 *\brief		Clone cet objet animable
		 *\param[out]	p_map	Re�oit les enfants clon�s en plus de l'objet clon�
		 *\return		Le clone
		 */
		MovingObjectBaseSPtr Clone( MovingObjectPtrStrMap & p_map );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapIt ScalesBegin()
		{
			return m_mapScales.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur constant sur le d�but de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapConstIt ScalesBegin()const
		{
			return m_mapScales.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapIt ScalesEnd()
		{
			return m_mapScales.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin de la map de keyframes
		 *\return		It�rateur constant sur la fin de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapConstIt ScalesEnd()const
		{
			return m_mapScales.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapIt TranslatesBegin()
		{
			return m_mapTranslates.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur constant sur le d�but de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapConstIt TranslatesBegin()const
		{
			return m_mapTranslates.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapIt TranslatesEnd()
		{
			return m_mapTranslates.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin de la map de keyframes
		 *\return		It�rateur constant sur la fin de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapConstIt TranslatesEnd()const
		{
			return m_mapTranslates.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapIt RotatesBegin()
		{
			return m_mapRotates.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur constant sur le d�but de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapConstIt RotatesBegin()const
		{
			return m_mapRotates.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la map de keyframes
		 *\return		It�rateur sur le d�but de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapIt RotatesEnd()
		{
			return m_mapRotates.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin de la map de keyframes
		 *\return		It�rateur constant sur la fin de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapConstIt RotatesEnd()const
		{
			return m_mapRotates.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation length
		 *\return		The animation length
		 *\~french
		 *\brief		R�cup�re la dur�e de l'animation
		 *\return		La dur�e de l'animation
		 */
		inline real GetLength()const
		{
			return m_rLength;
		}
		/**
		 *\~english
		 *\brief		Sets the animation length
		 *\param[in]	p_rLength	The new value
		 *\~french
		 *\brief		D�finit la dur�e de l'animation
		 *\param[in]	p_rLength	La nouvelle valeur
		 */
		inline void	SetLength( real p_rLength )
		{
			m_rLength = p_rLength;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re les animations du noeud de transformation
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetNodeTransform()const
		{
			return m_mtxNodeTransform;
		}
		/**
		 *\~english
		 *\brief		Sets the animation node transformation
		 *\param[in]	p_mtxTransform	The new value
		 *\~french
		 *\brief		D�finit les animations du noeud de transformation
		 *\param[in]	p_mtxTransform	La nouvelle valeur
		 */
		inline void SetNodeTransform( Castor::Matrix4x4r const & p_mtxTransform )
		{
			m_mtxNodeTransform = p_mtxTransform;
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
			return !m_mapRotates.empty() || !m_mapScales.empty() || !m_mapTranslates.empty();
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving thuing type
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le type du machin mouvant
		 *\return		La valeur
		 */
		inline eMOVING_OBJECT_TYPE GetType()const
		{
			return m_eType;
		}
		/**
		 *\~english
		 *\brief		Retrieves final transformation matrix for current animation time
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\return		La valeur
		 */
		inline const Castor::Matrix4x4r & GetFinalTransformation()const
		{
			return m_mtxFinalTransformation;
		}
		/**
		 *\~english
		 *\brief		Sets final transformation for current animation time
		 *\param[in]	p_mtx	The new value
		 *\~french
		 *\brief		D�finit la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\param[in]	p_mtx	La nouvelle valeur
		 */
		inline void SetFinalTransformation( const Castor::Matrix4x4r & p_mtx )
		{
			m_mtxFinalTransformation = p_mtx;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Iterator to the begin of the children array
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but du tableau d'enfants
		 *\return		It�rateur sur le d�but du tableau d'enfants
		 */
		inline MovingObjectPtrArrayIt ChildsBegin()
		{
			return m_arrayChildren.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Constant iterator to the begin of the children array
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but du tableau d'enfants
		 *\return		It�rateur constant sur le d�but du tableau d'enfants
		 */
		inline MovingObjectPtrArrayConstIt ChildsBegin()const
		{
			return m_arrayChildren.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Iterator to the begin of the children array
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but du tableau d'enfants
		 *\return		It�rateur sur le d�but du tableau d'enfants
		 */
		inline MovingObjectPtrArrayIt ChildsEnd()
		{
			return m_arrayChildren.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the children array
		 *\return		Constant iterator to the end of the children array
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin du tableau d'enfants
		 *\return		It�rateur constant sur la fin du tableau d'enfants
		 */
		inline MovingObjectPtrArrayConstIt ChildsEnd()const
		{
			return m_arrayChildren.end();
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix
		 *\~french
		 *\brief		Met � jour les transformations appliqu�es � l'objet
		 */
		virtual void DoApply() = 0;
		/**
		 *\~english
		 *\brief		Clones this moving thing
		 *\return		The clone
		 *\~french
		 *\brief		Clone cet objet animable
		 *\return		Le clone
		 */
		virtual MovingObjectBaseSPtr DoClone() = 0;

	private:
		Castor::Matrix4x4r DoComputeTransform( real p_rTime );
		Castor::Point3r DoComputeScaling( real p_rTime );
		Castor::Point3r DoComputeTranslation( real p_rTime );
		Castor::Quaternion DoComputeRotation( real p_rTime );

		template< class KeyFrameType >
		std::shared_ptr< KeyFrameType > DoAddKeyFrame( real p_rFrom, std::map< real, std::shared_ptr< KeyFrameType > > & p_map );

		template< class KeyFrameType >
		void DoRemoveKeyFrame( real p_rTime, std::map< real, std::shared_ptr< KeyFrameType > > & p_map );

		template< eINTERPOLATOR_MODE Mode, class ValueType, class KeyFrameType >
		ValueType DoCompute( real p_time, std::map< real, std::shared_ptr< KeyFrameType > > const & p_map, ValueType const & p_default );

	protected:
		//!\~english The key frames sorted by start time	\~french Les keyframes, tri�es par index de temps de d�but
		ScaleKeyFramePtrRealMap m_mapScales;
		//!\~english The key frames sorted by start time	\~french Les keyframes, tri�es par index de temps de d�but
		TranslateKeyFramePtrRealMap m_mapTranslates;
		//!\~english The key frames sorted by start time	\~french Les keyframes, tri�es par index de temps de d�but
		RotateKeyFramePtrRealMap m_mapRotates;
		//!\~english The animation length	\~french La dur�e de l'animation
		real m_rLength;
		//!\~english The objects depending on this one	\~french Les objets d�pendant de celui-ci
		MovingObjectPtrArray m_arrayChildren;
		//!\~english Animation node transformations	\~french Transformations du noeud d'animation
		Castor::Matrix4x4r m_mtxNodeTransform;
		//!\~english Actual transformations	\~french Transformations actuelles
		Castor::Matrix4x4r m_mtxTransformations;
		//!\~english The moving thing type	\~french Le type du machin mouvant
		eMOVING_OBJECT_TYPE m_eType;
		//!\~english The matrix holding transformation at current time	\~french La matrice de transformation compl�te au temps courant de l'animation
		Castor::Matrix4x4r m_mtxFinalTransformation;
	};
}

#include "MovingObjectBase.inl"

#pragma warning( pop )

#endif
