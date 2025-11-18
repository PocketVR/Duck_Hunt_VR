#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "./gun.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script {

    node_t vr_hand( string_t id, function_t<void,ref_t<node_t>> clb ) {
    return node::node_3D([=]( ref_t<node_t> self ){ 

        auto pos = self->get_attribute<transform_3D_t>( "transform"  );
        auto col = self->get_attribute<collision_t>   ( "collision"  );
        auto vis = self->get_attribute<visibility_t>  ( "visibility" );

        self->onLoop([=]( float delta ){
            if( !marker_list.has(id) ){ self->free(); return; }

            auto y   = marker_list[id];
            auto stm = y["timestmp"].as<ulong>();
            auto mat = y["data"]/**/.as<mat_t>();

            pos->rotation = math::quaternion::to_euler( math::quaternion::multiply(
                math::quaternion::from_axis  ({ vec3_t({ 0, 0, 1 }), PI }), 
                math::quaternion::from_matrix( mat )
            ));

            pos->scale   = vec3_t({ 1, 1, 1 });

            pos->position= rl::Vector3MoveTowards(
            pos->position, vec3_t({ mat.m12, mat.m13, mat.m14 })
                /*----*/ * vec3_t({ -2., -2., .8 }), 80. * delta
            );

            vis->mode = stm>process::now() ? visibility::MODE::VISIBILITY_MODE_ON :
            /*----------------------------*/ visibility::MODE::VISIBILITY_MODE_OFF;

        });

        self->append_child( script::gun( nullptr ) );

    clb(self); }); }

} }

/*────────────────────────────────────────────────────────────────────────────*/