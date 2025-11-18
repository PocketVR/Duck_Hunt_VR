#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "./effect.cpp"
#include "./rabbit.cpp"
#include "./bird.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace script {

    node_t enemy( function_t<void,ref_t<node_t>> clb ) {
    return node::node_3D([=]( ref_t<node_t> self ){

        auto snd = get_music_assets()["level" ]();
        self->onLoop([=]( float ){ snd.next(); });

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while( self->exists() ){ coYield(1); switch( rand() % 2 ){

                case 0 : do { self->append_child( script::rabbit([=]( ref_t<node_t> self ){

                    auto pos = self->get_attribute<transform_3D_t>( "transform" );
                    auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
                    auto nmb = rand() % 100;

                    pos->position = vec3_t({ sinf(nmb)*60, 3, cosf(nmb)*60 });
                    self->set_attribute( "target", vec3_t({ 
                        sinf(nmb) * 60, 3, cosf(nmb) * 60 
                    }) );

                }) ); } while(0); break;

                default: do { self->append_child( script::bird([=]( ref_t<node_t> self ){

                    auto pos = self->get_attribute<transform_3D_t>( "transform" );
                    auto shp = self->get_attribute<shape_3D_t>    ( "shape" );
                    auto nmb = rand() % 100;

                    pos->position = vec3_t({ sinf(nmb)*60, 50, cosf(nmb)*60 });
                    self->set_attribute( "target", vec3_t({ 
                        sinf(nmb) * 60, 50, cosf(nmb) * 60 
                    }) );

                }) ); } while(0); break;
                
            } 
                if( self->count_children() <= 10 ){ coGoto(1); } 
                coWait( self->count_children() > 10 ); 
                coDelay(1000); 
            }
        
        coFinish
        }));

    clb(self); }); }

} }

/*────────────────────────────────────────────────────────────────────────────*/