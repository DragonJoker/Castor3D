/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DPY_PREREQUISITES_H___
#define ___C3DPY_PREREQUISITES_H___

#include <Log/Logger.hpp>
#include <Math/Angle.hpp>
#include <Graphics/Glyph.hpp>
#include <Graphics/Font.hpp>
#include <Math/Quaternion.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Size.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Image.hpp>
#include <Graphics/PixelBuffer.hpp>
#include <Math/SquareMatrix.hpp>
#include <Math/Quaternion.hpp>

#include <Engine.hpp>
#include <Cache/AnimatedObjectGroupCache.hpp>
#include <Cache/BillboardCache.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/GeometryCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/OverlayCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/WindowCache.hpp>
#include <Animation/Animable.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Mesh/MeshAnimation.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Scene/Animation/AnimatedObject.hpp>
#include <Scene/Animation/AnimatedSkeleton.hpp>
#include <Scene/Animation/AnimatedMesh.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
#include <Scene/Animation/AnimationInstance.hpp>
#include <Scene/Animation/Mesh/MeshAnimationInstance.hpp>
#include <Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

namespace cpy
{
	template< typename Value, typename Class, typename Index >
	struct IndexedMemberGetter
	{
		typedef Value const & ( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		IndexedMemberGetter( Function p_function, Index p_index )
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
	struct IndexedMemberSetter
	{
		typedef Value & ( Class::*Function )( Index );
		Index m_index;
		Function m_function;
		IndexedMemberSetter( Function p_function, Index p_index )
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
	struct ParameteredMemberRefSetter
	{
		typedef void ( Class::*Function )( Index, Value const & );
		Index m_index;
		Function m_function;
		ParameteredMemberRefSetter( Function p_function, Index p_index )
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
	struct ParameteredMemberGetter
	{
		typedef Value( Class::*Function )( Index )const;
		Index m_index;
		Function m_function;
		ParameteredMemberGetter( Function p_function, Index p_index )
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
	struct ParameteredMemberSetter
	{
		typedef void ( Class::*Function )( Index, Value );
		Index m_index;
		Function m_function;
		ParameteredMemberSetter( Function p_function, Index p_index )
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
	struct MemberValueGetter
	{
		typedef Value( Class::*Function )()const;
		Function m_function;
		MemberValueGetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberValueSetter
	{
		typedef void ( Class::*Function )( Value );
		Function m_function;
		MemberValueSetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRefValueGetter
	{
		typedef Value const & ( Class::*Function )()const;
		Function m_function;
		MemberRefValueGetter( Function p_function )
			: m_function( p_function )
		{
		}
		Value const & operator()( Class * p_instance )
		{
			return ( p_instance->*( this->m_function ) )();
		}
	};

	template< typename Value, class Class >
	struct MemberRefValueSetter
	{
		typedef void ( Class::*Function )( Value const & );
		Function m_function;
		MemberRefValueSetter( Function p_function )
			: m_function( p_function )
		{
		}
		void operator()( Class * p_instance, Value const & p_value )
		{
			( p_instance->*( this->m_function ) )( p_value );
		}
	};

	template< typename Value, class Class >
	struct MemberRetValueSetter
	{
		typedef Value & ( Class::*Function )();
		Function m_function;
		MemberRetValueSetter( Function p_function )
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
		void operator()( Castor::Point3r * p_arg )
		{
			Castor::point::normalise( *p_arg );
		}
	};

	struct VectorNegater
	{
		void operator()( Castor::Point3r * p_arg )
		{
			Castor::point::negate( *p_arg );
		}
	};

	struct VectorLengther
	{
		void operator()( Castor::Point3r * p_arg )
		{
			Castor::point::distance( *p_arg );
		}
	};

	struct VectorDotter
	{
		Castor::real operator()( Castor::Point3r const & p_1, Castor::Point3r const & p_2 )
		{
			return Castor::point::dot( p_1, p_2 );
		}
	};

	struct VectorCrosser
	{
		Castor::Point3r operator()( Castor::Point3r const & p_1, Castor::Point3r const & p_2 )
		{
			return p_1 ^ p_2;
		}
	};

	struct PxBufferCreator
	{
		Castor::PxBufferBaseSPtr operator()( Castor::Size const & p_size, Castor::PixelFormat p_ePixelFormat )
		{
			return Castor::PxBufferBase::create( p_size, p_ePixelFormat );
		}
	};
	struct PointAdder
	{
		void operator()( Castor3D::Submesh * p_submesh, Castor::Point3r const & p_point )
		{
			p_submesh->AddPoint( p_point );
		}
	};

	struct FaceAdder
	{
		void operator()( Castor3D::Submesh * p_submesh, uint32_t a, uint32_t b, uint32_t c )
		{
			p_submesh->AddFace( a, b, c );
		}
	};

	struct Texture3DResizer
	{
		void operator()( Castor3D::TextureImage * texture, Castor::Size const & size, uint32_t depth )
		{
			texture->Resize( Castor::Point3ui( size.width(), size.height(), depth ) );
		}
	};

	template< class Texture, typename Param >
	struct Texture3DImageSetter
	{
		void operator()( Castor3D::TextureImage * texture, Castor::Size const & size, uint32_t depth, Param param )
		{
			texture->SetSource( Castor::Point3ui( size.width(), size.height(), depth ), param );
		}
	};

	struct GeometryCacheElementProducer
	{
		Castor3D::GeometrySPtr operator()( Castor3D::GeometryCache * p_cache, Castor::String const & p_key, Castor3D::SceneNodeSPtr p_node, Castor3D::MeshSPtr p_mesh )
		{
			return p_cache->Add( p_key, p_node, p_mesh );
		}
	};

	struct LightCacheElementProducer
	{
		Castor3D::LightSPtr operator()( Castor3D::LightCache * p_cache, Castor::String const & p_key, Castor3D::SceneNodeSPtr p_node, Castor3D::eLIGHT_TYPE p_type )
		{
			return p_cache->Add( p_key, p_node, p_type );
		}
	};

	struct MeshCacheElementProducer
	{
		Castor3D::MeshSPtr operator()( Castor3D::MeshCache * p_cache, Castor::String const & p_key )
		{
			return p_cache->Add( p_key );
		}
	};

	struct OverlayCacheElementProducer
	{
		Castor3D::OverlaySPtr operator()( Castor3D::OverlayCache * p_cache, Castor::String const & p_key, Castor3D::eOVERLAY_TYPE p_type, Castor3D::SceneSPtr p_scene, Castor3D::OverlaySPtr p_parent )
		{
			return p_cache->Add( p_key, p_type, p_scene, p_parent );
		}
	};

	template< typename Blend >
	struct BlendSetter
	{
		typedef void ( Castor3D::BlendState::*Function )( Blend, uint8_t );
		Function m_function;
		uint8_t m_index;
		BlendSetter( Function function, uint8_t index )
			:	m_function( function )
			,	m_index( index )
		{
		}
		void operator()( Castor3D::BlendState * state, Blend blend )
		{
			( state->*( this->m_function ) )( blend, m_index );
		}
	};

	template< class Texture, typename Param >
	Texture3DImageSetter< Texture, Param >
	make_image_setter( void ( Texture::* )( Castor::Point3ui const & sizes, Param param ) )
	{
		return Texture3DImageSetter< Texture, Param >();
	}

	template< typename Blend >
	BlendSetter< Blend >
	make_blend_setter( void ( Castor3D::BlendState::*function )( Blend, uint8_t ), uint8_t index )
	{
		return BlendSetter< Blend >( function, index );
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
			get_signature( cpy::IndexedMemberGetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::IndexedMemberSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::ParameteredMemberRefSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::ParameteredMemberGetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class, typename Index >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::ParameteredMemberSetter< Value, Class, Index >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value const &, Class * >
			get_signature( cpy::MemberRefValueGetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value const &, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value const & >
			get_signature( cpy::MemberRefValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value const & >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< Value, Class * >
			get_signature( cpy::MemberValueGetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< Value, Class * >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			template< typename Value, class Class >
			inline boost::mpl::vector< void, Class *, Value >
			get_signature( cpy::MemberRetValueSetter< Value, Class >, void * = 0 )
			{
				return boost::mpl::vector< void, Class *, Value >();
			}
			inline boost::mpl::vector< void, Castor::Point3r * >
			get_signature( cpy::VectorNormaliser, void * = 0 )
			{
				return boost::mpl::vector< void, Castor::Point3r * >();
			}
			inline boost::mpl::vector< void, Castor::Point3r * >
			get_signature( cpy::VectorNegater, void * = 0 )
			{
				return boost::mpl::vector< void, Castor::Point3r * >();
			}
			inline boost::mpl::vector< void, Castor::Point3r * >
			get_signature( cpy::VectorLengther, void * = 0 )
			{
				return boost::mpl::vector< void, Castor::Point3r * >();
			}
			inline boost::mpl::vector< Castor::real, Castor::Point3r const &, Castor::Point3r const & >
			get_signature( cpy::VectorDotter, void * = 0 )
			{
				return boost::mpl::vector< Castor::real, Castor::Point3r const &, Castor::Point3r const & >();
			}
			inline boost::mpl::vector< Castor::Point3r, Castor::Point3r const &, Castor::Point3r const & >
			get_signature( cpy::VectorCrosser, void * = 0 )
			{
				return boost::mpl::vector< Castor::Point3r, Castor::Point3r const &, Castor::Point3r const & >();
			}
			inline boost::mpl::vector< Castor::PxBufferBaseSPtr, Castor::Size const &, Castor::PixelFormat >
			get_signature( cpy::PxBufferCreator, void * = 0 )
			{
				return boost::mpl::vector< Castor::PxBufferBaseSPtr, Castor::Size const &, Castor::PixelFormat >();
			}
			inline boost::mpl::vector< Castor3D::PluginSPtr, Castor3D::Engine *, Castor::Path const & >
			get_signature( Castor3D::PluginSPtr ( Castor3D::Engine::* )( Castor::Path const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::PluginSPtr, Castor3D::Engine *, Castor::Path const & >();
			}
			inline boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const & >
			get_signature( Castor3D::MeshSPtr( Castor3D::Engine::* )( Castor3D::eMESH_TYPE, Castor::String const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const & >();
			}
			inline boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const & >
			get_signature( Castor3D::MeshSPtr( Castor3D::Engine::* )( Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const & >();
			}
			inline boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const &, Castor3D::RealArray const & >
			get_signature( Castor3D::MeshSPtr( Castor3D::Engine::* )( Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const &, Castor3D::RealArray const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::MeshSPtr, Castor3D::Engine *, Castor3D::eMESH_TYPE, Castor::String const &, Castor3D::UIntArray const &, Castor3D::RealArray const & >();
			}
			inline boost::mpl::vector< bool, Castor3D::Engine *, Castor3D::RenderWindowSPtr >
			get_signature( bool ( Castor3D::Engine::* )( Castor3D::RenderWindowSPtr ), void * = 0 )
			{
				return boost::mpl::vector< bool, Castor3D::Engine *, Castor3D::RenderWindowSPtr >();
			}
			inline boost::mpl::vector< Castor3D::OverlaySPtr, Castor3D::Engine *, Castor3D::eOVERLAY_TYPE, Castor::String const &, Castor3D::OverlaySPtr, Castor3D::SceneSPtr >
			get_signature( Castor3D::OverlaySPtr( Castor3D::Engine::* )( Castor3D::eOVERLAY_TYPE, Castor::String const &, Castor3D::OverlaySPtr, Castor3D::SceneSPtr ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::OverlaySPtr, Castor3D::Engine *, Castor3D::eOVERLAY_TYPE, Castor::String const &, Castor3D::OverlaySPtr, Castor3D::SceneSPtr >();
			}
			inline boost::mpl::vector< Castor3D::RenderWindowSPtr, Castor3D::Engine * >
			get_signature( Castor3D::RenderWindowSPtr( Castor3D::Engine::* )(), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::RenderWindowSPtr, Castor3D::Engine * >();
			}
			inline boost::mpl::vector< Castor3D::SceneSPtr, Castor3D::Engine *, Castor::String const & >
			get_signature( Castor3D::SceneSPtr( Castor3D::Engine::* )( Castor::String const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::SceneSPtr, Castor3D::Engine *, Castor::String const & >();
			}
			inline boost::mpl::vector< void, Castor3D::Submesh *, Castor::Point3r const & >
			get_signature( cpy::PointAdder, void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::Submesh *, Castor::Point3r const & >();
			}
			inline boost::mpl::vector< void, Castor3D::Submesh *, uint32_t, uint32_t, uint32_t >
			get_signature( cpy::FaceAdder, void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::Submesh *, uint32_t, uint32_t, uint32_t >();
			}
			inline boost::mpl::vector< Castor3D::MaterialSPtr, Castor3D::Geometry *, Castor3D::SubmeshSPtr >
			get_signature( Castor3D::MaterialSPtr( Castor3D::Geometry::* )( Castor3D::SubmeshSPtr ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::MaterialSPtr, Castor3D::Geometry *, Castor3D::SubmeshSPtr >();
			}
			inline boost::mpl::vector< void, Castor3D::Geometry *, Castor3D::SubmeshSPtr, Castor3D::MaterialSPtr >
			get_signature( void ( Castor3D::Geometry::* )( Castor3D::SubmeshSPtr, Castor3D::MaterialSPtr ), void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::Geometry *, Castor3D::SubmeshSPtr, Castor3D::MaterialSPtr >();
			}
			inline boost::mpl::vector< void, Castor3D::TextureImage *, Castor::Size const &, uint32_t >
			get_signature( cpy::Texture3DResizer, void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::TextureImage *, Castor::Size const &, uint32_t >();
			}
			template< class Texture, typename Param >
			inline boost::mpl::vector< void, Texture *, Castor::Size const &, uint32_t, Param >
			get_signature( cpy::Texture3DImageSetter< Texture, Param >, void * = 0 )
			{
				return boost::mpl::vector< void, Texture *, Castor::Size const &, uint32_t, Param >();
			}
			template< typename Blend >
			inline boost::mpl::vector< void, Castor3D::BlendState *, Blend >
			get_signature( cpy::BlendSetter< Blend >, void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::BlendState *, Blend >();
			}
			inline boost::mpl::vector< Castor3D::GeometrySPtr, Castor::String const &, Castor3D::SceneNodeSPtr, Castor3D::MeshSPtr >
			get_signature( cpy::GeometryCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::GeometrySPtr, Castor::String const &, Castor3D::SceneNodeSPtr, Castor3D::MeshSPtr >();
			}
			inline boost::mpl::vector< Castor3D::LightSPtr, Castor::String const &, Castor3D::SceneNodeSPtr, Castor3D::eLIGHT_TYPE >
			get_signature( cpy::LightCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::LightSPtr, Castor::String const &, Castor3D::SceneNodeSPtr, Castor3D::eLIGHT_TYPE >();
			}
			inline boost::mpl::vector< Castor3D::MeshSPtr, Castor::String const &, Castor3D::eMESH_TYPE, Castor3D::UIntArray, Castor3D::RealArray >
			get_signature( cpy::MeshCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::MeshSPtr, Castor::String const &, Castor3D::eMESH_TYPE, Castor3D::UIntArray, Castor3D::RealArray >();
			}
			inline boost::mpl::vector< Castor3D::OverlaySPtr, Castor::String const &, Castor3D::eOVERLAY_TYPE, Castor3D::SceneSPtr, Castor3D::OverlaySPtr >
			get_signature( cpy::OverlayCacheElementProducer, void * = 0 )
			{
				return boost::mpl::vector< Castor3D::OverlaySPtr, Castor::String const &, Castor3D::eOVERLAY_TYPE, Castor3D::SceneSPtr, Castor3D::OverlaySPtr >();
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
		return py::make_function( IndexedMemberGetter< Value, Class, Index >( p_function, Index( p_index ) ), py::return_value_policy< py::copy_const_reference >() );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberGetter< Value, Class, Index >
	make_getter( Value( Class::*p_function )( Index )const, _Index p_index )
	{
		return ParameteredMemberGetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename CallPolicies >
	inline py::object
	make_getter( Value const & ( Class::*p_function )()const, CallPolicies policies )
	{
		return py::make_function( MemberRefValueGetter< Value, Class >( p_function ), policies );
	}
	template< typename Value, class Class >
	inline MemberValueGetter< Value, Class >
	make_getter( Value( Class::*p_function )()const )
	{
		return MemberValueGetter< Value, Class >( p_function );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline IndexedMemberSetter< Value, Class, Index >
	make_setter( Value & ( Class::*p_function )( Index ), _Index p_index )
	{
		return IndexedMemberSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberSetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value ), _Index p_index )
	{
		return ParameteredMemberSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class, typename Index, typename _Index >
	inline ParameteredMemberRefSetter< Value, Class, Index >
	make_setter( void ( Class::*p_function )( Index, Value const & ), _Index p_index )
	{
		return ParameteredMemberRefSetter< Value, Class, Index >( p_function, Index( p_index ) );
	}
	template< typename Value, class Class >
	inline MemberRefValueSetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value const & ) )
	{
		return MemberRefValueSetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberValueSetter< Value, Class >
	make_setter( void ( Class::*p_function )( Value ) )
	{
		return MemberValueSetter< Value, Class >( p_function );
	}
	template< typename Value, class Class >
	inline MemberRetValueSetter< Value, Class >
	make_setter( Value & ( Class::*p_function )() )
	{
		return MemberRetValueSetter< Value, Class >( p_function );
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

			if ( it != x.end() )
			{
				return it->second;
			}

			KeyError();
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
