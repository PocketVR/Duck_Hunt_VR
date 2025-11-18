/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_COLLISION
#define UNGINE_COLLISION

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { struct overlap_3D_t {
    float  overlap = FLT_MAX;
    vec3_t axis    = {0,0,0};
    vec3_t point   = {0,0,0};
    float  sign    = 0.0f;
}; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { 
    
    bool is_overlaped( ptr_t<float> proj_a, ptr_t<float> proj_b ){
        if( proj_a.empty() || proj_b.empty() ){ return false; }
        return proj_a[0]<=proj_b[1] && proj_b[0]<=proj_a[1];
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { 

    ptr_t<vec3_t> get_3D_collision_axes( node_t a ){ do {
        auto pos = a.get_attribute<transform_3D_t>( "transform" );
        if ( pos == nullptr ){ break; }

        auto rot = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto out = ptr_t<vec3_t>( 3 );

        out[0] = vec3_t({ rot.m0, rot.m4, rot.m8  });
        out[1] = vec3_t({ rot.m1, rot.m5, rot.m9  });
        out[2] = vec3_t({ rot.m2, rot.m6, rot.m10 });

    return out; } while(0); return nullptr; }
    
    /*─······································································─*/

    ptr_t<float> get_3D_collision_projection( node_t a, vec3_t axis ){ do {

        auto pos  = a.get_attribute<transform_3D_t>( "transform" );
        auto col  = a.get_attribute<collision_t>   ( "collision" );
        auto shp  = a.get_attribute<shape_3D_t>    ( "shape" );

    if( pos==nullptr || col==nullptr || shp == nullptr ){ break; }
    if( col->mode & collision::MODE::COLLISION_MODE_BOX ){

        auto axes = get_3D_collision_axes( a ); if( axes.empty() ){ break; }

        float r = fabsf( Vector3DotProduct( axes[0], axis ) ) * pos->translate.scale.x/2 +
                  fabsf( Vector3DotProduct( axes[1], axis ) ) * pos->translate.scale.y/2 +
                  fabsf( Vector3DotProduct( axes[2], axis ) ) * pos->translate.scale.z/2 ;

        float c = Vector3DotProduct( pos->translate.position, axis );
        return ptr_t<float>({ c-r, c+r });

    } elif( col->mode & collision::MODE::COLLISION_MODE_RAY ) {

        auto rot   = math::matrix::rotation( math::negate( pos->translate.rotation ) );
        auto fr    = vec3_t({ rot.m2, rot.m6, rot.m10 }); 

        auto start = /*--------*/ pos->translate.position;
        auto stop  = start + fr * pos->translate.scale;

        float p_start = Vector3DotProduct( start, axis );
        float p_stop  = Vector3DotProduct( stop,  axis );

        float rmin = fminf( p_start, p_stop );
        float rmax = fmaxf( p_start, p_stop );
        
        return ptr_t<float>({ rmin, rmax });

    } else {

        float rmin=FLT_MAX; float rmax=-FLT_MAX;
        mat_t transform = GetTransformMatrix( 
            pos->translate.position, 
            pos->translate.scale   , 
            pos->translate.rotation 
        );

        for( int y=0; y<shp->model->get().meshCount; y++ ){
            
            rl::Mesh mesh = shp->model->get().meshes[y];
            int stride    = mesh.vertices ? 3 : 0;
            if( stride == 0 ){ continue; }
            
        for( int x=0; x<mesh.vertexCount; x++ ){
             int baseIndex = x * stride;
                
            if( baseIndex + 2 >= mesh.vertexCount * stride ){ break; }
                
            vec3_t vertex = {
                mesh.vertices[baseIndex + 0],
                mesh.vertices[baseIndex + 1], 
                mesh.vertices[baseIndex + 2]
            };
                
            vec3_t world_point = Vector3Transform (vertex, transform);
            float /*------*/ p = Vector3DotProduct(world_point, axis);
                
            rmin = fminf(rmin, p); rmax = fmaxf(rmax, p);
        }}  return ptr_t<float>({ rmin, rmax });

    }} while(0); return nullptr; }

    ptr_t<vec3_t> get_3D_collision_center( node_t a ) {
        auto pos = a.get_attribute<transform_3D_t>( "transform" );
        if ( pos == nullptr ){ return nullptr; }
        return type::bind( pos->translate.position );
    }
    
    /*─······································································─*/

    float get_3D_overlap_depth( node_t a, node_t b, vec3_t axis, float* sign ){

        auto proj_a = get_3D_collision_projection(a, axis);
        auto proj_b = get_3D_collision_projection(b, axis);
        auto cent_a = get_3D_collision_center(a);
        auto cent_b = get_3D_collision_center(b);

    if ( proj_a.null() || proj_b.null() || cent_a.null() || cent_b.null() )
       { return .0f; }

    if ( !is_overlaped( proj_a, proj_b ) ){ return .0f; }
        
        float overlap1 = proj_a[1] - proj_b[0];
        float overlap2 = proj_b[1] - proj_a[0];

        float depth = fmin( overlap1, overlap2 );

        float center_proj_a = Vector3DotProduct( *cent_a, axis );
        float center_proj_b = Vector3DotProduct( *cent_b, axis );
        
        if( depth == overlap1 ) {
                 *sign = center_proj_a > center_proj_b ? 1.0f :-1.0f;
        } else { *sign = center_proj_b > center_proj_a ?-1.0f : 1.0f; }
        
        return depth;
    }
    
    /*─······································································─*/

    ptr_t<overlap_3D_t> get_3D_collision( node_t a, node_t b ){ overlap_3D_t sign; do {

        auto axes_a = get_3D_collision_axes(a); if( axes_a.empty() ){ break; }
        auto axes_b = get_3D_collision_axes(b); if( axes_b.empty() ){ break; }

        for( auto& axis : axes_a ){ float n_sign = .0f;

            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }
        
        for( auto& axis : axes_b ){ float n_sign = .0f;

            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

        for( int i=0; i<3; ++i ){ for( int j=0; j<3; ++j ){

            vec3_t axis = Vector3CrossProduct( axes_a[i], axes_b[j] );

            if( Vector3Length(axis) < EPSILON ){ continue; }

            float n_sign  = .0f;
            float overlap = get_3D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }
            
        }}

    /*--*/ sign.point = Vector3Scale( sign.axis, sign.overlap * sign.sign );
    return type::bind( sign ); } while(0); return nullptr; }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { struct overlap_2D_t {
    float  overlap = FLT_MAX;
    vec2_t axis    = {0,0};
    vec2_t point   = {0,0};
    float  sign    = 0.0f;
}; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { 

    ptr_t<vec2_t> get_2D_collision_axes( node_t a ){ do {

        auto pos = a.get_attribute<transform_2D_t>( "transform" );
        if ( pos == nullptr ){ break; } 

        auto rot = pos->translate.rotation;
        
        return ptr_t<vec2_t>({
            vec2_t({  cosf(rot), sinf(rot) }),
            vec2_t({ -sinf(rot), cosf(rot) })
        });

    } while(0); return nullptr; }
    
    /*─······································································─*/

    ptr_t<float> get_2D_collision_projection( node_t a, vec2_t axis ){ do {

        auto pos  = a.get_attribute<transform_2D_t>( "transform" );
        auto col  = a.get_attribute<collision_t>   ( "collision" );
        auto shp  = a.get_attribute<shape_2D_t>    ( "shape" );

    if( pos==nullptr || col==nullptr || shp == nullptr ){ break; }
    if( col->mode & collision::MODE::COLLISION_MODE_BOX ){

        auto axes = get_2D_collision_axes( a ); if( axes.empty() ){ break; }

        float r = fabsf( Vector2DotProduct( axes[0], axis ) ) * pos->translate.scale.x/2 +
                  fabsf( Vector2DotProduct( axes[1], axis ) ) * pos->translate.scale.y/2 ;

        float c = Vector2DotProduct( pos->translate.position, axis );
        
        return ptr_t<float>({ c-r, c+r });
    
    } else {

        float rmin=FLT_MAX; float rmax=-FLT_MAX;
        ptr_t<vec2_t> points = shp->points;

        for( auto local_point: points ){

            vec2_t scaled_point  = Vector2Multiply( local_point  , pos->translate.scale    );
            vec2_t rotated_point = Vector2Rotate  ( scaled_point , pos->translate.rotation );
            vec2_t world_point   = Vector2Add     ( rotated_point, pos->translate.position );
                
            float p = Vector2DotProduct( world_point, axis );
                
            rmin = fminf( rmin,p ); rmax = fmaxf( rmax,p );
        }   return ptr_t<float>({ rmin, rmax });

    }} while(0); return nullptr; }

    ptr_t<vec2_t> get_2D_collision_center( node_t a ) {
        auto pos = a.get_attribute<transform_2D_t>( "transform" );
        if ( pos == nullptr ){ return nullptr; }
        return type::bind( pos->translate.position );
    }
    
    /*─······································································─*/

    float get_2D_overlap_depth( node_t a, node_t b, vec2_t axis, float* sign ){

        auto proj_a = get_2D_collision_projection(a, axis);
        auto proj_b = get_2D_collision_projection(b, axis);
        auto cent_a = get_2D_collision_center(a);
        auto cent_b = get_2D_collision_center(b);

    if ( proj_a.null() || proj_b.null() || cent_a.null() || cent_b.null() )
       { return .0f; }

    if ( !is_overlaped( proj_a, proj_b ) ){ return .0f; }
        
        float overlap1 = proj_a[1] - proj_b[0];
        float overlap2 = proj_b[1] - proj_a[0];

        float depth = fmin( overlap1, overlap2 );

        float center_proj_a = Vector2DotProduct( *cent_a, axis );
        float center_proj_b = Vector2DotProduct( *cent_b, axis );
        
        if( depth == overlap1 ) {
                 *sign = center_proj_a > center_proj_b ? 1.0f :-1.0f;
        } else { *sign = center_proj_b > center_proj_a ?-1.0f : 1.0f; }
        
        return depth;
    }
    
    /*─······································································─*/

    ptr_t<overlap_2D_t> get_2D_collision( node_t a, node_t b ){ overlap_2D_t sign; do {
    
        auto axes_a = get_2D_collision_axes(a); if( axes_a.empty() ){ break; }
        auto axes_b = get_2D_collision_axes(b); if( axes_b.empty() ){ break; }

        for( auto& axis : axes_a ){ float n_sign = .0f;

            float overlap = get_2D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

        for( auto& axis : axes_b ){ float n_sign = .0f;

            float overlap = get_2D_overlap_depth( a, b, axis, &n_sign ); 

            if( overlap < EPSILON      ){ return nullptr; }
            if( overlap < sign.overlap ){
                sign.overlap = overlap;
                sign.axis    = axis   ;
                sign.sign    = n_sign ;
            }

        }

    /*--*/ sign.point = Vector2Scale( sign.axis, sign.overlap * sign.sign );
    return type::bind( sign ); } while(0); return nullptr; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {

    node_t node_collision_line( function_t<void,ref_t<node_t>> clb ){
    return node_line([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_RAY;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_2D_t>( "transform"  );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_NONE;
             shp->color = rl::RED;

    clb( self ); }); }

    node_t node_collision_rectangle( function_t<void,ref_t<node_t>> clb ){
    return node_rectangle([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_BOX;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_2D_t>( "transform"  );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_NONE;
             shp->color = rl::RED;

    clb( self ); }); }

    node_t node_collision_circle( function_t<void,ref_t<node_t>> clb ){
    return node_circle([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_2D |
                        collision::MODE::COLLISION_MODE_SPH;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp );

        auto pos = self->get_attribute<transform_2D_t>( "transform"  );
        auto shp = self->get_attribute<shape_2D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_VERTEX;
             shp->color = rl::RED;

    clb( self ); }); }
    
    /*─······································································─*/

    node_t node_collision_ray( function_t<void,ref_t<node_t>> clb ){
    return node_ray([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_RAY;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_3D_t>( "transform"  );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_NONE;
             shp->color = rl::RED;

    clb( self ); }); }

    node_t node_collision_cube( function_t<void,ref_t<node_t>> clb ){
    return node_cube([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_BOX;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp ); 

        auto pos = self->get_attribute<transform_3D_t>( "transform"  );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_NONE;
             shp->color = rl::RED;

    clb( self ); }); }

    node_t node_collision_sphere( function_t<void,ref_t<node_t>> clb ){
    return node_sphere([=]( ref_t<node_t> self ){

        auto tmp /**/ = collision_t();
             tmp.mode = collision::MODE::COLLISION_MODE_3D |
                        collision::MODE::COLLISION_MODE_SPH;
             tmp.mask = collision::MASK::COLLISION_MASK_ALL;
        
        self->set_attribute( "collision", tmp );

        auto pos = self->get_attribute<transform_3D_t>( "transform"  );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
        
             shp->mode = shape::MODE::SHAPE_MODE_NONE;
             shp->color = rl::RED;
        
    clb( self ); }); }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace collision { void next( node_t a, node_t b ){

    auto mode = collision::MODE::COLLISION_MODE_2D | collision::MODE::COLLISION_MODE_3D;

    auto vis1 = a.get_attribute<visibility_t>( "visibility" );
    auto vis2 = b.get_attribute<visibility_t>( "visibility" );
    auto col1 = a.get_attribute<collision_t> ( "collision"  );
    auto col2 = b.get_attribute<collision_t> ( "collision"  );

    if(( col1->mode & col2->mode & mode )==0){ return; }
    if(( col1->mask & col2->mask /*--*/ )==0){ return; }

    if(  vis1->mode == 0x00 ) /*---*/ { return; }
    if(  vis2->mode == 0x00 ) /*---*/ { return; }
    if(( vis1->mask & vis2->mask )==0){ return; }

    if( col1->mode & collision::MODE::COLLISION_MODE_2D ){

        auto overlap = get_2D_collision( a, b );
        if( overlap.null() ){ return; }
        a.onCollision.emit( &b, any_t(*overlap) );

    } else {
        
        auto overlap = get_3D_collision( a, b );
        if( overlap.null() ){ return; }
        a.onCollision.emit( &b, any_t(*overlap) );
        
    }

}}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
