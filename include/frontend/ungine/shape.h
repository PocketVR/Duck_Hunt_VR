/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SHAPES
#define UNGINE_SHAPES

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { struct shape_2D_t {
    int mode /**/ = shape::MODE::SHAPE_MODE_FACES;
    color_t color = rl::WHITE;
    ptr_t<vec2_t>   points ;
    ptr_t<shader_t> shader ;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_polygon( ptr_t<vec2_t> points, function_t<void,ref_t<node_t>> clb ) {
    return node_2D( [=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_2D_t>( "transform" );
        auto tmp = shape_2D_t(); tmp.points = points;

        auto ptr = queue_t<vec2_t>();
        /*------*/ self->set_attribute( "shape", tmp );
        auto shp = self->get_attribute<shape_2D_t>( "shape" );

        self->onLoop([=]( float ){
            ptr.clear(); for( auto x: shp->points ){ 
            ptr.push( pos->translate.position + rl::Vector2Rotate(
            x * pos->translate.scale   ,
                pos->translate.rotation 
            )); } 
        });

        self->on2DDraw([=](){ if( ptr.empty() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }

        if( shp->mode & shape::MODE::SHAPE_MODE_FACES ){
            rl::DrawTriangleFan( ptr.data().get(), ptr.size(), shp->color );
        } elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES ) {
           for( auto x: ptr.data() ){ rl::DrawCircleV( x, 3, shp->color ); }
        } elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ 
           auto pt = ptr.data(); // console::log( "A" );
           for( int x=0; x<pt.size(); x++ )
              { rl::DrawLineV( pt[x],pt[x+1],shp->color ); }
        }

        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }

    /*─······································································─*/

    node_t node_polygon( int sides, function_t<void,ref_t<node_t>> clb ) {

        auto tmp = ptr_t<vec2_t>( sides );
        
        for( auto x=tmp.size(); x-->0; ){
             tmp[x].x = sin( 2*PI*x/tmp.size() );
             tmp[x].y = cos( 2*PI*x/tmp.size() );
        }

        return node_polygon( tmp, clb ); 
    
    }

    /*─······································································─*/

    node_t node_line( function_t<void,ref_t<node_t>> clb ) {
        return node_polygon( ptr_t<vec2_t>({
            vec2_t({ 0, 0 }), vec2_t({ 0, 1 })
        }), clb );
    }

    /*─······································································─*/

    node_t node_rectangle( function_t<void,ref_t<node_t>> clb ) {
        return node_polygon( ptr_t<vec2_t>({
            vec2_t({-1,-1 }), vec2_t({-1, 1 }),
            vec2_t({ 1, 1 }), vec2_t({ 1,-1 })
        }), clb );
    }

    /*─······································································─*/

    node_t node_triangle( function_t<void,ref_t<node_t>> clb ) { 
        return node_polygon( 3, clb );
    }

    node_t node_circle( function_t<void,ref_t<node_t>> clb ) { 
        return node_polygon( 16, clb );
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { struct shape_3D_t {
    int mode /**/ = shape::MODE::SHAPE_MODE_FACES;
    color_t color = rl::WHITE;
    ptr_t<model_t>  model ;
    ptr_t<shader_t> shader;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_model( model_t model, function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind( model );
             self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }

    node_t node_heightmap( image_t img, function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshHeightmap( img.get(), vec3_t({1,1,1}) ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }
    
    node_t node_cubicmap( image_t img, function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshCubicmap( img.get(), vec3_t({1,1,1}) ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }
    
    node_t node_cone( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshCylinder( 1, 1, 8 ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }
    
    node_t node_cylinder( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshCylinder( 1, 1, 8 ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }

    node_t node_quad( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshPlane( 1, 1, 1, 1 ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }
    
    node_t node_cube( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshCube( 1, 1, 1 ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }

    node_t node_sphere( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto tmp = shape_3D_t(); tmp.model = type::bind(
             model_t( rl::GenMeshSphere( 1, 8, 8 ) )
        );   self->set_attribute( "shape", tmp );

        auto shp = self->get_attribute<shape_3D_t>( "shape" );

        self->on3DDraw([=](){ if( shp->model.null() ){ return; }
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::BeginShaderMode( shp->shader->get() ); shp->shader->set_variables(); }
    
            if  ( shp->mode & shape::MODE::SHAPE_MODE_FACES  ){ shp->model->draw       ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_EDGES  ){ shp->model->draw_edges ( *pos, shp->color ); }
            elif( shp->mode & shape::MODE::SHAPE_MODE_VERTEX ){ shp->model->draw_vertex( *pos, shp->color ); }
            
        if( !shp->shader.null() && shp->shader->is_valid() )
          { rl::EndShaderMode(); }
        });

    clb( self ); }); }

    node_t node_ray( function_t<void,ref_t<node_t>> clb ) {
    return node_3D([=]( ref_t<node_t> self ){

        /*------*/ self->set_attribute( "shape", shape_3D_t() );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        auto pos = self->get_attribute<transform_3D_t>( "transform" );

        self->on3DDraw([=](){ if( shp->mode == 0x00 ){ return; }

        auto rot = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto fr  = vec3_t({ rot.m2, rot.m6, rot.m10 }); 

        if( shp->mode & ( shape::MODE::SHAPE_MODE_FACES | shape::MODE::SHAPE_MODE_EDGES ) ){
            rl::DrawLine3D( 
                /*--------------------------*/ pos->translate.position, 
                pos->translate.position + fr * pos->translate.scale
            ,   shp->color );
        } else {
            rl::DrawSphere( pos->translate.position /*----------------------*/ , 3, shp->color );
            rl::DrawSphere( pos->translate.position + fr * pos->translate.scale, 3, shp->color );
        }

        });

    clb(self); }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
