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
#ifndef ___C3D_Animation___
#define ___C3D_Animation___

#include "Prerequisites.hpp"

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
	\brief		Classe de représentation de choses mouvantes
	\remark		Gère les translations, mises à l'échelle, rotations de la chose
	*/
	typedef enum eMOVING_OBJECT_TYPE CASTOR_TYPE( uint8_t )
	{	eMOVING_OBJECT_TYPE_NODE
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
	\brief		Classe de représentation de choses mouvantes
	\remark		Gère les translations, mises à l'échelle, rotations de la chose
	*/
	class C3D_API MovingObjectBase
	{
	protected:
		//!\~english	The key frames sorted by start time	\~french	Les keyframes, triées par index de temps de début
		ScaleKeyFramePtrRealMap m_mapScales;
		//!\~english	The key frames sorted by start time	\~french	Les keyframes, triées par index de temps de début
		TranslateKeyFramePtrRealMap m_mapTranslates;
		//!\~english	The key frames sorted by start time	\~french	Les keyframes, triées par index de temps de début
		RotateKeyFramePtrRealMap m_mapRotates;
		//!\~english	The animation length	\~french	La durée de l'animation
		real m_rLength;
		//!\~english	The objects depending on this one	\~french	Les objets dépendant de celui-ci
		MovingObjectPtrArray m_arrayChildren;
		//!\~english	Animation node transformations	\~french	Transformations du noeud d'animation
		Castor::Matrix4x4r m_mtxNodeTransform;
		//!\~english	Actual transformations	\~french	Transformations actuelles
		Castor::Matrix4x4r m_mtxTransformations;
		//!\~english	The moving thing type	\~french	Le type du machin mouvant
		eMOVING_OBJECT_TYPE m_eType;
		//!\~english	The matrix holding transformation at current time	\~french	La matrice de transformation complète au temps courant de l'animation
		Castor::Matrix4x4r m_mtxFinalTransformation;

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
		 *\brief		Ajoute un objet enfant à celui-ci
		 *\remark		Les transformations de l'enfant sont affectées par celles de cet objet
		 *\param[in]	p_pObject	L'enfant
		 */
		void AddChild( MovingObjectBaseSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time
		 *\param[in]	p_rTime					Current time index
		 *\param[in]	p_bLooped				Tells if the animation is looped
		 *\param[în]	p_mtxTransformations	The current transformation matrix
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné
		 *\param[in]	p_rTime					Index de temps courant
		 *\param[in]	p_bLooped				Dit si l'animation est bouclée
		 *\param[în]	p_mtxTransformations	La matrice de transformation courante
		 */
		void Update( real p_rTime, bool p_bLooped, Castor::Matrix4x4r const & p_mtxTransformations );
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const = 0;
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_rFrom	L'index de temps de début
		 */
		ScaleKeyFrameSPtr AddScaleKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_rFrom	L'index de temps de début
		 */
		TranslateKeyFrameSPtr AddTranslateKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Creates a key frame and adds it to the list
		 *\remark		If a key frame with the same starting time already exists, it is returned, but not modified
		 *\param[in]	p_rFrom	The starting time
		 *\~french
		 *\brief		Crée une key frame et l'ajoute à la liste
		 *\remark		Si une key frame avec le même index de temps de début existe, elle est retournée sans être modifiée
		 *\param[in]	p_rFrom	L'index de temps de début
		 */
		RotateKeyFrameSPtr AddRotateKeyFrame( real p_rFrom );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
		 *\param[in]	p_rTime	L'index de temps
		 */
		void RemoveScaleKeyFrame( real p_rTime );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
		 *\param[in]	p_rTime	L'index de temps
		 */
		void RemoveTranslateKeyFrame( real p_rTime );
		/**
		 *\~english
		 *\brief		Deletes the key frame at time index p_time
		 *\param[in]	p_rTime	The time index
		 *\~french
		 *\brief		Supprime la key frame à l'index de temps donné
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
		 *\param[out]	p_map	Reçoit les enfants clonés en plus de l'objet cloné
		 *\return		Le clone
		 */
		MovingObjectBaseSPtr Clone( MovingObjectPtrStrMap & p_map );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapIt ScalesBegin() { return m_mapScales.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur constant sur le début de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapConstIt ScalesBegin()const { return m_mapScales.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapIt ScalesEnd() { return m_mapScales.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map de keyframes
		 *\return		Itérateur constant sur la fin de la map de keyframes
		 */
		inline ScaleKeyFramePtrRealMapConstIt ScalesEnd()const { return m_mapScales.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapIt TranslatesBegin() { return m_mapTranslates.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur constant sur le début de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapConstIt TranslatesBegin()const { return m_mapTranslates.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapIt TranslatesEnd() { return m_mapTranslates.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map de keyframes
		 *\return		Itérateur constant sur la fin de la map de keyframes
		 */
		inline TranslateKeyFramePtrRealMapConstIt TranslatesEnd()const { return m_mapTranslates.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapIt RotatesBegin() { return m_mapRotates.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Constant iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur constant sur le début de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapConstIt RotatesBegin()const { return m_mapRotates.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the keyframes map
		 *\return		Iterator to the begin of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la map de keyframes
		 *\return		Itérateur sur le début de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapIt RotatesEnd() { return m_mapRotates.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the keyframes map
		 *\return		Constant iterator to the end of the keyframes map
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map de keyframes
		 *\return		Itérateur constant sur la fin de la map de keyframes
		 */
		inline RotateKeyFramePtrRealMapConstIt RotatesEnd()const { return m_mapRotates.end(); }
		/**
		 *\~english
		 *\brief		Retrieves the animation length
		 *\return		The animation length
		 *\~french
		 *\brief		Récupère la durée de l'animation
		 *\return		La durée de l'animation
		 */
		inline real GetLength()const { return m_rLength; }
		/**
		 *\~english
		 *\brief		Sets the animation length
		 *\param[in]	p_rLength	The new value
		 *\~french
		 *\brief		Définit la durée de l'animation
		 *\param[in]	p_rLength	La nouvelle valeur
		 */
		inline void	SetLength( real p_rLength ) { m_rLength = p_rLength; }
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation
		 *\return		The value
		 *\~french
		 *\brief		Récupère les animations du noeud de transformation
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetNodeTransform()const { return m_mtxNodeTransform; }
		/**
		 *\~english
		 *\brief		Sets the animation node transformation
		 *\param[in]	p_mtxTransform	The new value
		 *\~french
		 *\brief		Définit les animations du noeud de transformation
		 *\param[in]	p_mtxTransform	La nouvelle valeur
		 */
		inline void SetNodeTransform( Castor::Matrix4x4r const & p_mtxTransform ) { m_mtxNodeTransform = p_mtxTransform; }
		/**
		 *\~english
		 *\brief		Tells whether or not the object has keyframes
		 *\return		\p false if no keyframes
		 *\~french
		 *\brief		Dit si l'objet a des keyframes
		 *\return		\p false si pas de keyframes
		 */
		inline bool HasKeyFrames()const { return !m_mapRotates.empty() || !m_mapScales.empty() || !m_mapTranslates.empty(); }
		/**
		 *\~english
		 *\brief		Retrieves the moving thuing type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type du machin mouvant
		 *\return		La valeur
		 */
		inline eMOVING_OBJECT_TYPE GetType()const { return m_eType; }
		/**
		 *\~english
		 *\brief		Retrieves final transformation matrix for current animation time
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\return		La valeur
		 */
		inline const Castor::Matrix4x4r & GetFinalTransformation()const { return m_mtxFinalTransformation; }
		/**
		 *\~english
		 *\brief		Sets final transformation for current animation time
		 *\param[in]	p_mtx	The new value
		 *\~french
		 *\brief		Définit la matrice finale de transformation du bone pour le temps courant de l'animation
		 *\param[in]	p_mtx	La nouvelle valeur
		 */
		inline void SetFinalTransformation( const Castor::Matrix4x4r & p_mtx ) { m_mtxFinalTransformation = p_mtx; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Iterator to the begin of the children array
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau d'enfants
		 *\return		Itérateur sur le début du tableau d'enfants
		 */
		inline MovingObjectPtrArrayIt ChildsBegin() { return m_arrayChildren.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Constant iterator to the begin of the children array
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau d'enfants
		 *\return		Itérateur constant sur le début du tableau d'enfants
		 */
		inline MovingObjectPtrArrayConstIt ChildsBegin()const { return m_arrayChildren.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the children array
		 *\return		Iterator to the begin of the children array
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau d'enfants
		 *\return		Itérateur sur le début du tableau d'enfants
		 */
		inline MovingObjectPtrArrayIt ChildsEnd() { return m_arrayChildren.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the children array
		 *\return		Constant iterator to the end of the children array
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'enfants
		 *\return		Itérateur constant sur la fin du tableau d'enfants
		 */
		inline MovingObjectPtrArrayConstIt ChildsEnd()const { return m_arrayChildren.end(); }

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet
		 */
		virtual void DoApply()=0;
		/**
		 *\~english
		 *\brief		Clones this moving thing
		 *\return		The clone
		 *\~french
		 *\brief		Clone cet objet animable
		 *\return		Le clone
		 */
		virtual MovingObjectBaseSPtr DoClone()=0;

	private:
		Castor::Matrix4x4r DoCompute( real p_rTime );
		Castor::Point3r DoComputeScaling( real p_rTime );
		Castor::Point3r DoComputeTranslation( real p_rTime );
		Castor::Quaternion DoComputeRotation( real p_rTime );
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for abstract nodes
	\remark		Used to decompose the model and place intermediate animations
	\~french
	\brief		Implémentation de MovingObjectBase pour des noeuds abstraits.
	\remark		Utilisé afin de décomposer le modèle et ajouter des animatiobns intermédiaires
	*/
	class C3D_API MovingNode : public MovingObjectBase
	{
	private:
		static const Castor::String StrVoidString;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovingNode();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingNode();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone();
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for MovableObject
	\~french
	\brief		Implémentation de MovingObjectBase pour les MovableObject
	*/
	class C3D_API MovingObject : public MovingObjectBase
	{
	private:
		//!\~english	The object affected by the animations	\~french	L'objet affecté par les animations
		MovableObjectWPtr m_pObject;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovingObject();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingObject();
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_pObject	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_pObject	L'objet
		 */
		inline void SetObject( MovableObjectSPtr p_pObject ) { m_pObject = p_pObject; }
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline MovableObjectSPtr GetObject()const { return m_pObject.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone();
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for Bones.
	\~french
	\brief		Implémentation de MovingObjectBase pour les Bones.
	*/
	class C3D_API MovingBone : public MovingObjectBase
	{
	private:
		//!\~english	The bone affected by the animations	\~french	L'os affecté par les animations
		BoneWPtr m_pBone;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovingBone();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingBone();
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_pObject	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_pObject	L'objet
		 */
		inline void SetBone( BoneSPtr p_pBone ) { m_pBone = p_pBone; }
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline BoneSPtr GetBone()const { return m_pBone.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone();
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\todo		Review all the animation system because it's not clear, not optimised, and not good enough to be validated
	\todo		Write and Read functions.
	\~english
	\brief		Animation class
	\remark		The class which handles an Animation, its length, key frames ...
	\~french
	\brief		Classe d'animation
	\remark		Classe gérant une Animation, sa durée, les key frames ...
	*/
	class C3D_API Animation : public Castor::Named
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		09/02/2010
		\~english
		\brief		Animation State Enum
		\remark		The enumeration which defines all the states of an animation : playing, stopped, paused
		\~french
		\brief		Enumération des états d'une animation
		*/
		typedef enum eSTATE CASTOR_TYPE( uint8_t )
		{	eSTATE_PLAYING	//!< Playing animation state
		,	eSTATE_STOPPED	//!< Stopped animation state
		,	eSTATE_PAUSED	//!< Paused animation state
		,	eSTATE_COUNT
		}	eSTATE;

	protected:
		//!\~english	The current playing time	\~french	L'index de temps courant
		real m_rCurrentTime;
		//!\~english	The current state of the animation	\~french	L'état actuel de l'animation
		eSTATE m_eState;
		//!\~english	The animation time scale	\~french	Le multiplicateur de temps
		real m_rScale;
		//!\~english	The animation length	\~french	La durée de l'animation
		real m_rLength;
		//!\~english	Tells whether or not the animation is looped	\~french	Dit si oui ou non l'animation est bouclée
		bool m_bLooped;
		//! The moving objects
		MovingObjectPtrStrMap m_mapToMove;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_strName	The name of the animation
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_strName	Le nom de l'animation
		 */
		Animation( Castor::String const & p_strName=Castor::cuEmptyString);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Animation();
		/**
		 *\~english
		 *\brief		Creates and adds an animated node
		 *\~french
		 *\brief		Crée et ajoute un noeud animé
		 */
		MovingObjectBaseSPtr AddMovingObject();
		/**
		 *\~english
		 *\brief		Creates and adds an animated bone
		 *\param[in]	p_pObject		The bone to add
		 *\~french
		 *\brief		Crée et ajoute un os animé
		 *\param[in]	p_pObject		L'os
		 */
		MovingObjectBaseSPtr AddMovingObject( BoneSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Creates and adds an animated object
		 *\param[in]	p_pObject		The movable object to add
		 *\~french
		 *\brief		Crée et ajoute un objet animé
		 *\param[in]	p_pObject		L'objet déplaçable
		 */
		MovingObjectBaseSPtr AddMovingObject( MovableObjectSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Adds an animated object
		 *\param[in]	p_pObject		The animated object to add
		 *\~french
		 *\brief		Ajoute un objet animé
		 *\param[in]	p_pObject		L'objet animé
		 */
		void AddMovingObject( MovingObjectBaseSPtr p_pObject );
		/**
		 *\~english
		 *\brief		Retrieves an animated bone
		 *\param[in]	p_pObject		The bone to add
		 *\~french
		 *\brief		Récupère un os animé
		 *\param[in]	p_pObject		L'os
		 */
		MovingObjectBaseSPtr GetMovingObject( BoneSPtr p_pObject )const;
		/**
		 *\~english
		 *\brief		Retrieves an animated object
		 *\param[in]	p_pObject		The movable object to add
		 *\~french
		 *\brief		Récupère un objet animé
		 *\param[in]	p_pObject		L'objet déplaçable
		 */
		MovingObjectBaseSPtr GetMovingObject( MovableObjectSPtr p_pObject )const;
		/**
		 *\~english
		 *\brief		Updates the animation, updates the key frame at the good time index
		 *\param[in]	p_rTslf	The time since the last frame
		 *\~french
		 *\brief		Met l'animation à jour, met à jour les key frames aux bons index de temps
		 *\param[in]	p_rTslf	Le temps écoulé depuis la dernière frame
		 */
		void Update( real p_rTslf );
		/**
		 *\~english
		 *\brief		Plays the animation
		 *\~french
		 *\brief		Démarre l'animation
		 */
		void Play();
		/**
		 *\~english
		 *\brief		Pauses the animation
		 *\~french
		 *\brief		Met l'animation en pause
		 */
		void Pause();
		/**
		 *\~english
		 *\brief		Stops the animation
		 *\~french
		 *\brief		Stoppe l'animation
		 */
		void Stop();
		/**
		 *\~english
		 *\brief		Retrieves the animation state
		 *\return		The animation state
		 *\~french
		 *\brief		Récupère l'état de l'animation
		 *\return		L'état de l'animation
		 */
		inline eSTATE GetState()const { return m_eState; }
		/**
		 *\~english
		 *\brief		Retrieves the animation time scale
		 *\return		The animation time scale
		 *\~french
		 *\brief		Récupère le multiplicateur de temps de l'animation
		 *\return		Le multiplicateur de temps de l'animation
		 */
		inline real GetScale()const { return m_rScale; }
		/**
		 *\~english
		 *\brief		Retrieves the animation loop status
		 *\return		The animation loop status
		 *\~french
		 *\brief		Récupère l'état de boucle de l'animation
		 *\return		L'état de boucle de l'animation
		 */
		inline bool IsLooped()const { return m_bLooped; }
		/**
		 *\~english
		 *\brief		Sets the animation time scale
		 *\param[in]	p_rScale	The new value
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation
		 *\param[in]	p_rScale	La nouvelle valeur
		 */
		inline void	SetScale( real p_rScale ) { m_rScale = p_rScale; }
		/**
		 *\~english
		 *\brief		Sets the animation loop status
		 *\param[in]	p_bLooped	The new value
		 *\~french
		 *\brief		Définit l'état de boucle de l'animation
		 *\param[in]	p_bLooped	La nouvelle valeur
		 */
		inline void SetLooped( bool p_bLooped ) { m_bLooped = p_bLooped; }
		/**
		 *\~english
		 *\brief		Retrieves the moving objects count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre d'objets mouvants
		 *\return		Le nombre
		 */
		inline uint32_t GetMovingObjectsCount()const { return uint32_t( m_mapToMove.size() ); }
		/**
		 *\~english
		 *\brief		Retrieves an interator on the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapIt Begin() { return m_mapToMove.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant interator on the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapConstIt Begin()const { return m_mapToMove.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an interator on the end of the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapIt End() { return m_mapToMove.end(); }
		/**
		 *\~english
		 *\brief		Retrieves a constant interator on the end of the moving objects map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin de la map d'objets mouvants
		 *\return		L'itérateur
		 */
		inline MovingObjectPtrStrMapConstIt End()const { return m_mapToMove.end(); }
	};
}

#pragma warning( pop )

#endif
