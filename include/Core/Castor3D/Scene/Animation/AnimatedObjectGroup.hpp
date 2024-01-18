/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_OBJECT_GROUP_H___
#define ___C3D_ANIMATED_OBJECT_GROUP_H___

#include "AnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Data/Loader.hpp>
#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	class AnimatedObjectGroup
		: public castor::Named
		, public castor::OwnedBy< Scene >
	{
	public:
		using AnimatedObjectMap = castor::StringMap< AnimatedObjectUPtr >;
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	scene	The scene
		 *\param[in]	name	The group name
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	scene	La scène
		 *\param[in]	name	Le nom du groupe
		 */
		C3D_API AnimatedObjectGroup( castor::String const & name
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~AnimatedObjectGroup()noexcept;
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the SceneNode given as a parameter, adds it to the list.
		 *\param[in]	node	The SceneNode from which AnimatedObject is created.
		 *\param[in]	name	The SceneNode instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du SceneNode donné, l'ajoute à la liste.
		 *\param[in]	node	Le SceneNode à partir duquel l'AnimatedObject est créé.
		 *\param[in]	name	Le nom de l'instance du SceneNode.
		 */
		C3D_API AnimatedObjectRPtr addObject( SceneNode & node
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the Mesh given as a parameter, adds it to the list.
		 *\param[in]	mesh		The Mesh from which AnimatedObject is created.
		 *\param[in]	geometry	The geometry instantiating the mesh.
		 *\param[in]	name		The Mesh instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du Mesh donné, l'ajoute à la liste.
		 *\param[in]	mesh		Le Mesh à partir duquel l'AnimatedObject est créé.
		 *\param[in]	geometry	La géométrie instanciant le maillage.
		 *\param[in]	name		Le nom de l'instance du Mesh.
		 */
		C3D_API AnimatedObjectRPtr addObject( Mesh & mesh
			, Geometry & geometry
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject from the Skeleton given as a parameter, adds it to the list.
		 *\param[in]	skeleton	The Skeleton from which AnimatedObject is created.
		 *\param[in]	mesh		The mesh to which the skeleton is bound.
		 *\param[in]	geometry	The geometry instantiating the mesh.
		 *\param[in]	name		The Skeleton instance name.
		 *\~french
		 *\brief		Crée un AnimatedObject à partir du Skeleton donné, l'ajoute à la liste.
		 *\param[in]	skeleton	Le Skeleton à partir duquel l'AnimatedObject est créé.
		 *\param[in]	mesh		Le maillage auquel le squelette est lié.
		 *\param[in]	geometry	La géométrie instanciant le maillage.
		 *\param[in]	name		Le nom de l'instance du Skeleton.
		 */
		C3D_API AnimatedObjectRPtr addObject( Skeleton & skeleton
			, Mesh & mesh
			, Geometry & geometry
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Creates an AnimatedObject for a texture, adds it to the list.
		 *\param[in]	sourceInfo	The texture source informations.
		 *\param[in]	config		The texture configuration.
		 *\param[in]	pass		The pass instanciating the texture.
		 *\~french
		 *\brief		Crée un AnimatedObject pour une texture, l'ajoute à la liste.
		 *\param[in]	sourceInfo	Les informations de source de la texture.
		 *\param[in]	config		La configuration de texture
		 *\param[in]	pass		La passe instanciant la texture.
		 */
		C3D_API AnimatedObjectRPtr addObject( TextureSourceInfo const & sourceInfo
			, TextureConfiguration const & config
			, Pass & pass );
		/**
		 *\~english
		 *\brief		adds an AnimatedObject to this group.
		 *\remarks		This function assumes the object has already been created in order to be put in this group (with the good scene and animations map)
		 *\param[in]	object	The AnimatedObject to add
		 *\~french
		 *\brief		Ajoute un AnimatedObject à ce groupe.
		 *\remarks		Cette fonction considère que l'objet a préalablement été créé afin d'être intégré à ce groupe (avec donc les bonnes scène et map d'animations)
		 *\param[in]	object	Le AnimatedObject à ajouter
		 */
		C3D_API bool addObject( AnimatedObjectUPtr object );
		/**
		 *\~english
		 *\brief		Find an AnimatedObject given its name.
		 *\param[in]	name	The AnimatedObject name.
		 *\~french
		 *\brief		Recherche un AnimatedObject en fonction de son nom.
		 *\param[in]	name	Le nom de l'AnimatedObject.
		 */
		C3D_API AnimatedObject * findObject( castor::String const & name )const;
		/**
		 *\~english
		 *\brief		adds the animation to the list.
		 *\param[in]	name	The animation to add.
		 *\return		\p true if the animation didn't exist already.
		 *\~french
		 *\brief		Ajoute une animation à la liste.
		 *\param[in]	name	L'animation à ajouter.
		 *\return		\p true si l'animation n'existait pas encore.
		 */
		C3D_API bool addAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Sets the loop status of wanted animation.
		 *\param[in]	name	The name of the animation.
		 *\param[in]	looped	The status.
		 *\~french
		 *\brief		Définit le statut de bouclage de l'animation voulue.
		 *\param[in]	name	Le nom de l'animation.
		 *\param[in]	looped	Le statut.
		 */
		C3D_API void setAnimationLooped( castor::String const & name
			, bool looped );
		/**
		 *\~english
		 *\brief		Sets the time scale of wanted animation.
		 *\param[in]	name	The name of the animation.
		 *\param[in]	scale	The scale.
		 *\~french
		 *\brief		Définit le multiplicateur de temps de l'animation voulue.
		 *\param[in]	name	Le nom de l'animation.
		 *\param[in]	scale	Le multiplicateur.
		 */
		C3D_API void setAnimationScale( castor::String const & name
			, float scale );
		/**
		 *\~english
		 *\brief		Sets the starting point of wanted animation.
		 *\param[in]	name	The name of the animation.
		 *\param[in]	value	The starting point.
		 *\~french
		 *\brief		Définit le point de départ de l'animation voulue.
		 *\param[in]	name	Le nom de l'animation.
		 *\param[in]	value	Le point de départ.
		 */
		C3D_API void setAnimationStartingPoint( castor::String const & name
			, castor::Milliseconds value );
		/**
		 *\~english
		 *\brief		Sets the stopping point of wanted animation.
		 *\param[in]	name	The name of the animation.
		 *\param[in]	value	The stopping point.
		 *\~french
		 *\brief		Définit le point d'arrêt de l'animation voulue.
		 *\param[in]	name	Le nom de l'animation.
		 *\param[in]	value	Le point d'arrêt.
		 */
		C3D_API void setAnimationStoppingPoint( castor::String const & name
			, castor::Milliseconds value );
		/**
		 *\~english
		 *\brief		Sets the interpolation mode of wanted animation.
		 *\param[in]	name	The name of the animation.
		 *\param[in]	mode	The interpolation mode.
		 *\~french
		 *\brief		Définit le mode d'interpolation de l'animation voulue.
		 *\param[in]	name	Le nom de l'animation.
		 *\param[in]	mode	Le mode d'interpolation.
		 */
		C3D_API void setAnimationInterpolation( castor::String const & name
			, InterpolatorType mode );
		/**
		 *\~english
		 *\brief			CPU Update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Starts the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Démarre l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void startAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Stops the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Stoppe l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void stopAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Pauses the animation identified by the given name
		 *\param[in]	name	The name of the animation
		 *\~french
		 *\brief		Met en pause l'animation identifiée par le nom donné
		 *\param[in]	name	Le nom de l'animation
		 */
		C3D_API void pauseAnimation( castor::String const & name );
		/**
		 *\~english
		 *\brief		Starts all animations
		 *\~french
		 *\brief		Démarre toutes les animations
		 */
		C3D_API void startAllAnimations();
		/**
		 *\~english
		 *\brief		Stops all animations
		 *\~french
		 *\brief		Stoppe toutes les animations
		 */
		C3D_API void stopAllAnimations();
		/**
		 *\~english
		 *\brief		Pauses all animations
		 *\~french
		 *\brief		Met en pause toutes les animations
		 */
		C3D_API void pauseAllAnimations();
		/**
		 *\~english
		 *\return		The animations map.
		 *\~french
		 *\return		La map d'animations.
		 */
		GroupAnimationMap const & getAnimations()const
		{
			return m_animations;
		}
		/**
		 *\~english
		 *\return		The animated objects.
		 *\~french
		 *\return		Les objets animés.
		 */
		AnimatedObjectMap const & getObjects()const
		{
			return m_objects;
		}

	public:
		OnAnimatedSkeletonChange onSkeletonAdded;
		OnAnimatedSkeletonChange onSkeletonRemoved;
		OnAnimatedMeshChange onMeshAdded;
		OnAnimatedMeshChange onMeshRemoved;
		OnAnimatedTextureChange onTextureAdded;
		OnAnimatedTextureChange onTextureRemoved;
		OnAnimatedSceneNodeChange onSceneNodeAdded;
		OnAnimatedSceneNodeChange onSceneNodeRemoved;

	private:
		GroupAnimationMap m_animations;
		AnimatedObjectMap m_objects;
		castor::PreciseTimer m_timer;
	};

	struct SceneContext;

	struct AnimGroupContext
	{
		SceneContext * scene{};
		AnimatedObjectGroupRPtr animGroup{};
		AnimatedObjectRPtr animSkeleton{};
		AnimatedObjectRPtr animMesh{};
		AnimatedObjectRPtr animNode{};
		AnimatedObjectRPtr animTexture{};
		castor::String animName{};
	};

	C3D_API Engine * getEngine( AnimGroupContext const & context );
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::InterpolatorType >
	{
		static inline xchar const * const Name = cuT( "InterpolatorType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::InterpolatorType >();
				return result;
			}( );
	};
}

#endif
