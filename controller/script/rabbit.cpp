#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script { 

    node_t rabbit( function_t<void,ref_t<node_t>> clb ){
    return node::node_collision_cube([=]( ref_t<node_t> self ){ clb(self); 

        auto pos = self->get_attribute<transform_3D_t>( "transform" );
        auto shp = self->get_attribute<shape_3D_t>    ( "shape"  );
        auto trg = self->get_attribute<vec3_t>        ( "target" );
        auto prn = self->get_parent();

        auto snd = get_music_assets()["level" ]();
        auto mdl = get_model_assets()["rabbit"]();

        auto hlh = type::bind( new int( 1 ) );
             pos->scale = vec3_t({ 3, 3, 3 });

        /*---*/
        self->set_attribute( "enemy", function_t<void>([=](){
            *hlh -= 1; if( *hlh <= 0 ){ timer::timeout([=](){
                self->get_root()->append_child(effect(&pos));
                snd.shot(); self->free();
            }, 700 ); }
        }));
        /*---*/

        self->append_child( node::node_model( mdl, [=]( ref_t<node_t> self ){

            auto pos = self->get_attribute<transform_3D_t>( "transform" );
            auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
            auto tme = type::bind( new float( 0.0f ) );

                 pos->scale = vec3_t({ 3, 3, 3 });

            self->onLoop([=]( float delta ){ *tme += delta * 7;
                pos->position.y = fabsf(sinf(*tme)) - 3;
            });

        }) );

    //  self->on3DDraw([=](){ rl::DrawCube( *trg, 3, 3, 3, rl::WHITE ); });
        self->onLoop  ([=]( float delta ){

            auto distance = math::distance( pos->translate.position, *trg );
            
            do{ vec3_t x = *trg - pos->translate.position;
                float  y = atan2f( x.x, x.z ) - PI/2;
                pos->rotation = math::move_toward(
                pos->rotation, vec3_t({ 0, y, 0 }), 3.0f * delta );
            } while(0);

            pos->position += math::vec3::rotation( vec3_t({ 1, 0, 0 }), 
            /*------------*/ math::quaternion::from_euler( pos->rotation ) 
            ) * delta * 10.f;

            if( distance < 3 ){ auto nmb = rand() % 100; 
                *trg = vec3_t({ sinf(nmb)*60, 3, cosf(nmb)*60 }); 
            } elif( distance > 200 ) { self->free(); }

        });

    }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/