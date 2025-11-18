#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script { node_t effect( transform_3D_t* npos ) {

    return node::node_sphere([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
             pos->position = npos->translate.position;
             pos->scale    = vec3_t({ 3, 3, 3 });

        self->onLoop([=]( float delta ){
            pos->scale = math::move_toward( pos->scale, vec3_t({ 0, 0, 0 }), delta * 10 );
            if( math::length( pos->scale ) < EPSILON ){ self->free(); }
        });

    }); 

}}}

/*────────────────────────────────────────────────────────────────────────────*/