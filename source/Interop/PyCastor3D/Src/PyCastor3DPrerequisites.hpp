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
#ifndef ___C3DPY_PREREQUISITES_H___
#define ___C3DPY_PREREQUISITES_H___

#include <Logger.hpp>
#include <Angle.hpp>
#include <Glyph.hpp>
#include <Font.hpp>
#include <Quaternion.hpp>
#include <Position.hpp>
#include <Size.hpp>
#include <Rectangle.hpp>
#include <Image.hpp>
#include <PixelBufferBase.hpp>
#include <SquareMatrix.hpp>
#include <Quaternion.hpp>

#include <Animable.hpp>
#include <AnimatedObject.hpp>
#include <AnimatedObjectGroup.hpp>
#include <Engine.hpp>
#include <SceneManager.hpp>
#include <Camera.hpp>
#include <MeshManager.hpp>
#include <Submesh.hpp>
#include <Light.hpp>
#include <DirectionalLight.hpp>
#include <SpotLight.hpp>
#include <PointLight.hpp>
#include <MaterialManager.hpp>
#include <Pass.hpp>
#include <PluginManager.hpp>
#include <TextureUnit.hpp>
#include <StaticTexture.hpp>
#include <DynamicTexture.hpp>
#include <ShaderProgram.hpp>
#include <SamplerManager.hpp>
#include <Geometry.hpp>
#include <WindowManager.hpp>
#include <OverlayManager.hpp>
#include <PanelOverlay.hpp>
#include <BorderPanelOverlay.hpp>
#include <TextOverlay.hpp>
#include <BlendStateManager.hpp>
#include <DepthStencilStateManager.hpp>
#include <RasteriserStateManager.hpp>

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
		Castor::PxBufferBaseSPtr operator()( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat )
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
		void operator()( Castor3D::DynamicTexture * texture, Castor::Size const & size, uint32_t depth )
		{
			texture->Resize( Castor::Point3ui( size.width(), size.height(), depth ) );
		}
	};

	template< class Texture, typename Param >
	struct Texture3DImageSetter
	{
		void operator()( Texture * texture, Castor::Size const & size, uint32_t depth, Param param )
		{
			texture->SetImage( Castor::Point3ui( size.width(), size.height(), depth ), param );
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
			inline boost::mpl::vector< Castor::PxBufferBaseSPtr, Castor::Size const &, Castor::ePIXEL_FORMAT >
			get_signature( cpy::PxBufferCreator, void * = 0 )
			{
				return boost::mpl::vector< Castor::PxBufferBaseSPtr, Castor::Size const &, Castor::ePIXEL_FORMAT >();
			}
			inline boost::mpl::vector< Castor3D::PluginBaseSPtr, Castor3D::Engine *, Castor::Path const & >
			get_signature( Castor3D::PluginBaseSPtr( Castor3D::Engine::* )( Castor::Path const & ), void * = 0 )
			{
				return boost::mpl::vector< Castor3D::PluginBaseSPtr, Castor3D::Engine *, Castor::Path const & >();
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
			inline boost::mpl::vector< void, Castor3D::DynamicTexture *, Castor::Size const &, uint32_t >
			get_signature( cpy::Texture3DResizer, void * = 0 )
			{
				return boost::mpl::vector< void, Castor3D::DynamicTexture *, Castor::Size const &, uint32_t >();
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
}

extern void ExportCastorUtils();
extern void ExportCastor3D();

#endif

