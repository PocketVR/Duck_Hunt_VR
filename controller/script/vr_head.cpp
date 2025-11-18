#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "./vr_hand.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script { 

    node_t vr_head( function_t<void, ref_t<node_t>> clb ){
    return node::node_camera_3D([=]( ref_t<node_t> self ){

        auto viw = self->get_viewport(); viw->mode = visibility::MODE::VISIBILITY_MODE_VR;

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto cam = self->get_attribute<camera_3D_t>   ( "camera" );
             pos->position = vec3_t({ 0, 10, 0 });

        self->onLoop([=]( float delta ){

            auto mot /**/ = get_sensor_data();
            auto rot /**/ = math::quaternion::multiply(
                math::quaternion::from_axis({ { 1, 0 , 0 }, PI/2 }), 
                vec4_t({ mot.y, mot.x,-mot.z, mot.w })
            );

            pos->rotation = math::quaternion::to_euler( rot );

        });

        self->onNext([=](){

            ([=](){ coStart do { 

                for( auto x: marker_list.keys() ){
                     auto y= regex::format ( "hand_${0}", x );
                if ( self->has_child( y ) ){ continue; }
                     self->append_child( y, vr_hand( x, nullptr ));
                }

            } while(0); coDelay(1000); coGoto(0); coStop })();

        });

    clb(self); }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/