#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "../script/enemy.cpp"
#include "../script/vr_head.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace scene { node_t scene_0() {

    return node::node_scene([=]( ref_t<node_t> self ){

        auto mdl = get_model_assets()["scene"]();
        auto snd = get_music_assets()["scene"](); snd.play();
        
        self->append_child( "scene", node::node_model( mdl, [=]( ref_t<node_t> self ){
            auto pos = self->get_attribute<transform_3D_t>( "transform" );
                 pos->scale *= 10; cursor::lock();
        }) );

        self->append_child( "vr_head", script::vr_head( nullptr ) );
        self->append_child( "enemy"  , script::enemy  ( nullptr ) );

    //  self->append_child( node::node_fly_camera_3D( nullptr ) );

        self->on2DDraw([=](){ rl::DrawFPS ( 30, 30  ); });
    //  self->on3DDraw([=](){ rl::DrawGrid( 10, 100 ); });

        self->onLoop([=]( float ){ snd.next(); });

    });

}}}

/*────────────────────────────────────────────────────────────────────────────*/