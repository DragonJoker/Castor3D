/* See LICENSE file in root folder */
#ifndef ___C3DPY_PREREQUISITES_H___
#define ___C3DPY_PREREQUISITES_H___

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Graphics/Glyph.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/PixelBuffer.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/BillboardCache.hpp>
#include <Castor3D/Cache/CameraCache.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/SceneCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Animation/Animable.hpp>
#include <Castor3D/Animation/Skeleton/SkeletonAnimation.hpp>
#include <Castor3D/Animation/Mesh/MeshAnimation.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/LegacyPass.hpp>
#include <Castor3D/Mesh/Mesh.hpp>
#include <Castor3D/Mesh/Submesh.hpp>
#include <Castor3D/Mesh/SubmeshComponent/LinesMapping.hpp>
#include <Castor3D/Mesh/SubmeshComponent/TriFaceMapping.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/BillboardList.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedSkeleton.hpp>
#include <Castor3D/Scene/Animation/AnimatedMesh.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Animation/AnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Texture/Sampler.hpp>
#include <Castor3D/Texture/TextureView.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>

namespace cpy
{
	template< typename Value, typename Class, typename Index >
	struct IndexedMembergetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		IndexedMembergetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		Value const & operator()( Class * p_value )
		{
			return ( p_value->*( this->m_function ) )( m_index );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct IndexedMembersetter
	{
		typedef Value & ( Class::*Function )( Index );
		Index m_index;
		Function m_function;
		IndexedMembersetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value const & p_arg )
		{
			( p_value->*( this->m_function ) )( m_index ) = p_arg;
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMemberRefsetter
	{
		typedef void ( Class::*Function )( Index, Value const & );
		Index m_index;
		Function m_function;
		ParameteredMemberRefsetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value const & p_arg )
		{
			( p_value->*( this->m_function ) )( m_index, p_arg );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMembergetter
	{
		typedef Value( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		ParameteredMembergetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		Value operator()( Class * p_value )
		{
			return ( p_value->*( this->m_function ) )( m_index );
		}
	};

	template< typename Value, typename Class, typename Index >
	struct ParameteredMembersetter
	{
		typedef void ( Class::*Function )( Index, Value );
		Index m_index;
		Function m_function;
		ParameteredMembersetter( Function p_function, Index p_index )
			:	m_index( p_index )
			,	m_function( p_function )
		{
		}
		void operator()( Class * p_value, Value p_arg )
		{
			( p_value->*( this->m_function ) )( m_index, p_arg );
		}
	};

	template< typename Value, class Class >
	struct MemberValuegetter
	{
		typedef Value( Class::*Function )()const;
		Function m_function;
		MemberValuegetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberValuesetter
	{
		typedef void ( Class::*Function )( Value );
		Function m_function;
		MemberValuesetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRefValuegetter
	{
		typedef Value const & ( Class::*Function )()const;
		Function m_function;
		MemberRefValuegetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value const & operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberRefValuesetter
	{
		typedef void ( Class::*Function )( Value const & );
		Function m_function;
		MemberRefValuesetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value const & p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRetValuesetter
	{
		typedef Value & ( Class::*Function )();
		Function m_function;
		MemberRetValuesetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value p_value )
		{
			( p_instance->*( this->m_function ) )() = p_value;
		}
	};

	struct VectorNormaliser
	{
		void operator()( castor::Point3r * p_arg )
		{
			castor::point::normalise( *p_arg );
		}
	};

	struct VectorNegater
	{
		void operator()( castor::Point3r * p_arg )
		{
			castor::point::negate( *p_arg );
		}
	};

	struct VectorLengther
	{
		void operator()( castor::Point3r * p_arg )
		{
			castor::point::length( *p_arg );
		}
	};

	struct Vectordotter
	{
		castor::real operator()( castor::Point3r const & p_1, castor::Point3r const & p_2 )
		{
			return castor::point::dot( p_1, p_2 );
		}
	};

	struct VectorCrosser
	{
		castor::Point3r operator()( castor::Point3r const & p_1, castor::Point3r const & p_2 )
		{
			return castor::point::cross( p_1, p_2 );
		}
	};

	struct PxBufferCreator
	{
		castor::PxBufferBaseSPtr operator()( castor::Size const & p_size, castor::PixelFormat p_ePixelFormat )
		{
			return castor::PxBufferBase::create( p_size, p_ePixelFormat );
		}
	};
	struct PointAdder
	{
		void operator()( castor3d::Submesh * p_submesh, castor::Point3r const & p_point )
		{
			p_submesh->addPoint( p_point );
		}
	};

	struct FaceAdder
	{
		void operator()( castor3d::TriFaceMapping * p_mapping, uint32_t a, uint32_t b, uint32_t c )
		{
			p_mapping->addFace( a, b, c );
		}
	};

	struct LineAdder
	{
		void operator()( castor3d::LinesMapping * p_mapping, uint32_t a, uint32_t b )
		{
			p_mapping->addLine( a, b );
		}
	};

	template< class Texture, typename Param >
	struct Texture3DImageSetter
	{
		void operator()( castor3d::TextureView * texture, castor::Size const & size, uint32_t depth, Param param )
		{
			texture->setSource( castor::Point3ui( size.getWidth(), size.getHeight(), depth ), param );
		}
	};

	struct GeometryCacheElementProducer
	{
		castor3d::GeometrySPtr operator()( castor3d::GeometryCache * p_cache, castor::String const & p_key, castor3d::SceneNodeSPtr p_node, castor3d::MeshSPtr p_mesh )
		{
			return p_cache->add( p_key, p_node, p_mesh );
		}
	};

	struct LightCacheElementProducer
	{
		castor3d::LightSPtr operator()( castor3d::LightCache * p_cache, castor::String const & p_key, castor3d::SceneNodeSPtr p_node, castor3d::LightType p_type )
		{
			return p_cache->add( p_key, p_node, p_type );
		}
	};

	struct MeshCacheElementProducer
	{
		castor3d::MeshSPtr operator()( castor3d::MeshCache * p_cache, castor::String const & p_key )
		{
			return p_cache->add( p_key );
		}
	};

	struct OverlayCacheElementProducer
	{
		castor3d::OverlaySPtr operator()( castor3d::OverlayCache * p_cache, castor::String const & p_key, castor3d::OverlayType p_type, castor3d::SceneSPtr p_scene, castor3d::OverlaySPtr p_parent )
		{
			return p_cache->add( p_key, p_type, p_scene, p_parent );
		}
	};

	struct CameraCacheElementProducer
	{
		castor3d::CameraSPtr operator()( castor3d::CameraCache * p_cache, castor::String const & p_key, castor3d::SceneNodeSPtr p_node, castor3d::Viewport && p_viewport )
		{
			return p_cache->add( p_key, p_node, std::move( p_viewport ) );
		}
	};

	struct QuaternionFromAxisAngle
	{
		castor::Quaternion operator()( castor::Point3f const & axis
			, castor::Angle const & angle )
		{
			return castor::Quaternion::fromAxisAngle( axis, angle );
		}
	};

	struct QuaternionFromAxes
	{
		castor::Quaternion operator()( castor::Point3f const & x
			, castor::Point3f const & y
			, castor::Point3f const & z )
		{
			return castor::Quaternion::fromAxes( x, y, z );
		}
	};

	struct QuaternionFromMatrix
	{
		castor::Quaternion operator()( castor::Matrix4x4r const & matrix )
		{
			return castor::Quaternion::fromMatrix( matrix );
		}
	};

	struct FillRgb
	{
		castor::Image & operator()( castor::Image & image, castor::RgbColour const & rgb )
		{
			return image.fill( rgb );
		}
	};

	struct FillRgba
	{
		castor::Image & operator()( castor::Image & image, castor::RgbaColour const & rgba )
		{
			return image.fill( rgba );
		}
	};

	template< class Texture, typename Param >
	Texture3DImageSetter< Texture, Param >
	make_image_setter( void ( Texture::* )( castor::Point3ui const & sizes, Param param ) )
	{
		return Texture3DImageSetter< Texture, Param >();
	}
}

#include <boost/mpl/vector.hpp>

namespace boost
{
	namespace python
	{
		namespace detail
		{
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< Value const &, Class * >
			get_signature( cpy::IndexedMembergetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::IndexedMembersetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::ParameteredMemberRefsetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::ParameteredMembergetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::ParameteredMembersetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value const &, Class * >
			get_signature( cpy::MemberRefValuegetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::MemberRefValuesetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::MemberValuegetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberValuesetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberRetValuesetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			inline boost::mpl::vector< void, castor::Point3r * >
			get_signature( cpy::VectorNormaliser, void * = 0 )
			{
				return boost::mpl::vector< void, castor::Point3r * >();
			}
			inline boost::mpl::vector< void, castor::Point3r * >
			get_signature( cpy::VectorNegater, void * = 0 )
			{
				return boost::mpl::vector< void, castor::Point3r * >();
			}
			inline boost::mpl::vector< void, castor::Point3r * >
			get_signature( cpy::VectorLengther, void * = 0 )
			{
				return boost::mpl::vector< void, castor::Point3r * >();
			}
			inline boost::mpl::vector< castor::real, castor::Point3r const &, castor::Point3r const & >
			get_signature( cpy::Vectordotter, void * = 0 )
			{
				return boost::mpl::vector< castor::real, castor::Point3r const &, castor::Point3r const & >();
			}
			inline boost::mpl::vector< castor::Point3r, castor::Point3r const &, castor::Point3r const & >
			get_signature( cpy::VectorCrosser, void * = 0 )
			{
				return boost::mpl::vector< castor::Point3r, castor::Point3r const &, castor::Point3r const & >();
			}
			inline boost::mpl::vector< castor::PxBufferBaseSPtr, castor::Size const &, castor::PixelFormat >
			get_signature( cpy::PxBufferCreator, void * = 0 )
			{
				return boost::mpl::vector< castor::PxBufferBaseSPtr, castor::Size const &, castor::PixelFormat >();
			}
			inline boost::mpl::vector< castor3d::PluginSPtr, castor3d::Engine *, castor::Path const & >
			get_signature( castor3d::PluginSPtr ( castor3d::Engine::* )( castor::Path const & ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::PluginSPtr, castor3d::Engine *, castor::Path const & >();
			}
			inline boost::mpl::vector< castor3d::MeshSPtr, castor3d::Engine *, castor::String const &, castor::String const & >
			get_signature( castor3d::MeshSPtr( castor3d::Engine::* )( castor::String const &, castor::String const & ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::MeshSPtr, castor3d::Engine *, castor::String const &, castor::String const & >();
			}
			inline boost::mpl::vector< castor3d::MeshSPtr, castor3d::Engine *, castor::String const &, castor::String const &, castor3d::Parameters const & >
			get_signature( castor3d::MeshSPtr( castor3d::Engine::* )( castor::String const &, castor::String const &, castor3d::Parameters const & ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::MeshSPtr, castor3d::Engine *, castor::String const &, castor::String const &, castor3d::Parameters const & >();
			}
			inline boost::mpl::vector< bool, castor3d::Engine *, castor3d::RenderWindowSPtr >
			get_signature( bool ( castor3d::Engine::* )( castor3d::RenderWindowSPtr ), void * = 0 )
			{
				return boost::mpl::vector< bool, castor3d::Engine *, castor3d::RenderWindowSPtr >();
			}
			inline boost::mpl::vector< castor3d::OverlaySPtr, castor3d::Engine *, castor3d::OverlayType, castor::String const &, castor3d::OverlaySPtr, castor3d::SceneSPtr >
			get_signature( castor3d::OverlaySPtr( castor3d::Engine::* )( castor3d::OverlayType, castor::String const &, castor3d::OverlaySPtr, castor3d::SceneSPtr ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::OverlaySPtr, castor3d::Engine *, castor3d::OverlayType, castor::String const &, castor3d::OverlaySPtr, castor3d::SceneSPtr >();
			}
			inline boost::mpl::vector< castor3d::RenderWindowSPtr, castor3d::Engine * >
			get_signature( castor3d::RenderWindowSPtr( castor3d::Engine::* )(), void * = 0 )
			{
				return boost::mpl::vector< castor3d::RenderWindowSPtr, castor3d::Engine * >();
			}
			inline boost::mpl::vector< castor3d::SceneSPtr, castor3d::Engine *, castor::String const & >
			get_signature( castor3d::SceneSPtr( castor3d::Engine::* )( castor::String const & ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::SceneSPtr, castor3d::Engine *, castor::String const & >();
			}
			inline boost::mpl::vector< void, castor3d::Submesh *, castor::Point3r const & >
			get_signature( cpy::PointAdder, void * = 0 )
			{
				return boost::mpl::vector< void, castor3d::Submesh *, castor::Point3r const & >();
			}
			inline boost::mpl::vector< void, castor3d::TriFaceMapping *, uint32_t, uint32_t, uint32_t >
			get_signature( cpy::FaceAdder, void * = 0 )
			{
				return boost::mpl::vector< void, castor3d::TriFaceMapping *, uint32_t, uint32_t, uint32_t >();
			}
			inline boost::mpl::vector< void, castor3d::LinesMapping *, uint32_t, uint32_t >
			get_signature( cpy::LineAdder, void * = 0 )
			{
				return boost::mpl::vector< void, castor3d::LinesMapping *, uint32_t, uint32_t >();
			}
			inline boost::mpl::vector< castor3d::MaterialSPtr, castor3d::Geometry *, castor3d::SubmeshSPtr >
			get_signature( castor3d::MaterialSPtr( castor3d::Geometry::* )( castor3d::SubmeshSPtr ), void * = 0 )
			{
				return boost::mpl::vector< castor3d::MaterialSPtr, castor3d::Geometry *, castor3d::SubmeshSPtr >();
			}
			inline boost::mpl::vector< void, castor3d::Geometry *, castor3d::SubmeshSPtr, castor3d::MaterialSPtr >
			get_signature( void ( castor3d::Geometry::* )( castor3d::SubmeshSPtr, castor3d::MaterialSPtr ), void * = 0 )
			{
				return boost::mpl::vector< void, castor3d::Geometry *, castor3d::SubmeshSPtr, castor3d::MaterialSPtr >();
			}
			template< class Texture, typename Param >
			inline boost::mpl::vector< void, Texture *, castor::Size const &, uint32_t, Param >
			get_signature( cpy::Texture3DImageSetter< Texture, Param >, void * = 0 )
			{
				return boost::mpl::vector< void, Texture *, castor::Size const &, uint32_t, Param >();
			}
			inline boost::mpl::vector< castor3d::GeometrySPtr, castor::String const &, castor3d::SceneNodeSPtr, castor3d::MeshSPtr >
			get_signature( cpy::GeometryCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< castor3d::GeometrySPtr, castor::String const &, castor3d::SceneNodeSPtr, castor3d::MeshSPtr >();
			}
			inline boost::mpl::vector< castor3d::LightSPtr, castor::String const &, castor3d::SceneNodeSPtr, castor3d::LightType >
			get_signature( cpy::LightCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< castor3d::LightSPtr, castor::String const &, castor3d::SceneNodeSPtr, castor3d::LightType >();
			}
			inline boost::mpl::vector< castor3d::MeshSPtr, castor::String const &, castor::String const &, castor3d::Parameters const & >
			get_signature( cpy::MeshCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< castor3d::MeshSPtr, castor::String const &, castor::String const &, castor3d::Parameters const & >();
			}
			inline boost::mpl::vector< castor3d::OverlaySPtr, castor::String const &, castor3d::OverlayType, castor3d::SceneSPtr, castor3d::OverlaySPtr >
			get_signature( cpy::OverlayCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< castor3d::OverlaySPtr, castor::String const &, castor3d::OverlayType, castor3d::SceneSPtr, castor3d::OverlaySPtr >();
			}
			inline boost::mpl::vector< castor::Quaternion, castor::Point3f const &, castor::Angle const & >
			get_signature( cpy::QuaternionFromAxisAngle, void * = 0 )
			{
				return boost::mpl::vector< castor::Quaternion, castor::Point3f const &, castor::Angle const & >();
			}
			inline boost::mpl::vector< castor::Quaternion, castor::Point3f const &, castor::Point3f const &, castor::Point3f const & >
			get_signature( cpy::QuaternionFromAxes, void * = 0 )
			{
				return boost::mpl::vector< castor::Quaternion, castor::Point3f const &, castor::Point3f const &, castor::Point3f const & >();
			}
			inline boost::mpl::vector< castor::Quaternion, castor::Matrix4x4r const & >
			get_signature( cpy::QuaternionFromMatrix, void * = 0 )
			{
				return boost::mpl::vector< castor::Quaternion, castor::Matrix4x4r const & >();
			}
			inline boost::mpl::vector< castor::Image &, castor::Image &, castor::RgbColour const & >
			get_signature( cpy::FillRgb, void * = 0 )
			{
				return boost::mpl::vector< castor::Image &, castor::Image &, castor::RgbColour const & >();
			}
			inline boost::mpl::vector< castor::Image &, castor::Image &, castor::RgbaColour const & >
			get_signature( cpy::FillRgba, void * = 0 )
			{
				return boost::mpl::vector< castor::Image &, castor::Image &, castor::RgbaColour const & >();
			}
			inline boost::mpl::vector< castor3d::CameraSPtr, castor3d::CameraCache *, castor::String const &, castor3d::SceneNodeSPtr, castor3d::Viewport && >
			get_signature( cpy::CameraCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< castor3d::CameraSPtr, castor3d::CameraCache *, castor::String const &, castor3d::SceneNodeSPtr, castor3d::Viewport && >();
			}
		}
	}
}

#include <boost/python.hpp>
namespace py = boost::python;

namespace cpy
{
	template< typename Value, class Class, typename Index, typename _Index >
	inline py::object
	make_getter( Value const & ( Class::*p_function )( Index )const, _Index p_index )
	{
		return py::make_function( IndexedMembergetter< Value, Class, Index >( p_function, Index( p_index ) ), py::return_value_policy< py::copy_const_reference >() );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMembergetter< Value, Class, Index >
	make_getter( Value( Class::*p_function )( Index )const, _Index p_index )
	{
		return ParameteredMembergetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename CallPolicies >
	inline py::object
	make_getter( Value const & ( Class::*p_function )()const, CallPolicies policies )
	{
		return py::make_function( MemberRefValuegetter< Value, Class >( p_function ), policies );
	}
	template< typename Value, class Class >
	inline MemberValuegetter< Value, Class >
	make_getter( Value( Class::*p_function )()const )
	{
		return MemberValuegetter< Value, Class >( p_function );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline IndexedMembersetter< Value, Class, Index >
	make_setter( Value & ( Class::*p_function )( Index ), _Index p_index )
	{
		return IndexedMembersetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMembersetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value ), _Index p_index )
	{
		return ParameteredMembersetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberRefsetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value const & ), _Index p_index )
	{
		return ParameteredMemberRefsetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class >
	inline MemberRefValuesetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value const & ) )
	{
		return MemberRefValuesetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberValuesetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value ) )
	{
		return MemberValuesetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberRetValuesetter< Value, Class >
	make_setter( Value & ( Class::*p_function )() )
	{
		return MemberRetValuesetter< Value, Class >( p_function );
	}

	inline void IndexError()
	{
		PyErr_SetString( PyExc_IndexError, "Index out of range" );
	}

	template< class T >
	struct std_vector_wrapper
	{
		using vector_type = T;
		using value_type = typename vector_type::value_type;

		static inline value_type & get( vector_type & x, int i )
		{
			if ( i < 0 )
			{
				i += x.size();
			}

			if ( i >= 0 && i < x.size() )
			{
				return x[i];
			}

			IndexError();
		}

		static inline void set( vector_type & x, int i, value_type const & v )
		{
			if ( i < 0 )
			{
				i += x.size();
			}

			if ( i >= 0 && i < x.size() )
			{
				x[i] = v;
			}
			else
			{
				IndexError();
			}
		}

		static inline void del( vector_type & x, int i )
		{
			if ( i < 0 )
			{
				i += x.size();
			}

			if ( i >= 0 && i < x.size() )
			{
				x.erase( i );
			}
			else
			{
				IndexError();
			}
		}

		static inline void add( vector_type & x, value_type const & v )
		{
			x.push_back( v );
		}
	};

	template< class Cont, class Func >
	inline py::class_< Cont > make_vector_wrapper( std::string const & p_name, Func p_accessor )
	{
		return py::class_< Cont >( p_name.c_str() )
			   .def( "__len__", &Cont::size )
			   .def( "clear", &Cont::clear )
			   .def( "append", &std_vector_wrapper< Cont >::add,
					 py::with_custodian_and_ward<1, 2>() ) // to let container keep value
			   .def( "__getitem__", &std_vector_wrapper< Cont >::get,
					 py::return_value_policy< py::copy_non_const_reference >() )
			   .def( "__setitem__", &std_vector_wrapper< Cont >::set,
					 py::with_custodian_and_ward< 1, 2 >() ) // to let container keep value
			   .def( "__delitem__", &std_vector_wrapper< Cont >::del )
			   ;
	}

	inline void KeyError()
	{
		PyErr_SetString( PyExc_KeyError, "Key not found" );
	}

	template< class T >
	struct std_map_wrapper
	{
		using map_type = T;
		using key_type = typename map_type::key_type;
		using mapped_type = typename map_type::mapped_type;

		static inline mapped_type & get( map_type & x, key_type const & i )
		{
			auto it = x.find( i );

			if ( it == x.end() )
			{
				KeyError();
			}

			return it->second;
		}

		static inline void set( map_type & x, key_type const & i, mapped_type const & v )
		{
			x[i] = v;
		}

		static inline void del( map_type & x, key_type const & i )
		{
			auto it = x.find( i );

			if ( it != x.end() )
			{
				x.erase( i );
			}
			else
			{
				KeyError();
			}
		}
	};

	template< class Cont, class Func >
	inline py::class_< Cont > make_map_wrapper( std::string const & p_name, Func p_accessor )
	{
		return py::class_< Cont >( p_name.c_str() )
			   .def( "__len__", &Cont::size )
			   .def( "clear", &Cont::clear )
			   .def( "__getitem__", &std_map_wrapper< Cont >::get,
					 py::return_value_policy< py::copy_non_const_reference >() )
			   .def( "__setitem__", &std_map_wrapper< Cont >::set,
					 py::with_custodian_and_ward< 1, 2 >() ) // to let container keep value
			   .def( "__delitem__", &std_map_wrapper< Cont >::del )
			   ;
	}
}

extern void ExportCastorUtils();
extern void ExportCastor3D();

#endif
