#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script {

    node_t gun( function_t<void,ref_t<node_t>> clb ){
    return node::node_3D([=]( ref_t<node_t> self ){

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto mdl = get_model_assets()["pistol"]();
        auto snd = get_music_assets()["pistol"]();
             pos->scale = vec3_t({ 3, 3, 3 });

        ptr_t<bool> collided = new bool( false );

        self->on3DDraw([=](){ mdl.draw( *pos, rl::WHITE ); });
        self->onLoop  ([=]( float ){ snd.next(); });

        self->onLoop  ([=]( float delta ){
            pos->rotation = math::move_toward( pos->rotation, vec3_t({ 0, 0, 0 }), delta * 2.f );
            if( pos->rotation.x < EPSILON && !snd.is_playing() ){
            if( *collided ){ pos->rotation.x = PI/4; *collided =false; }}
        });

        self->append_child( node::node_collision_ray([=]( ref_t<node_t> self ){

            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
                 shp->mode     = shape::MODE::SHAPE_MODE_EDGES;
                 pos->scale    = vec3_t({ 3000, 3000, 3000 });
                 pos->rotation = vec3_t({    0,   PI,    0 });
            
            self->onCollision([=]( node_t* node, any_t ev ){
                if( !node->has_attribute( "enemy" ) || *collided ){ return; }
                auto clb = node->get_attribute<function_t<void>>("enemy");
                *collided = true; snd.shot(); clb->emit();
            });

        }));

    clb(self); }); }

} }

/*────────────────────────────────────────────────────────────────────────────*/