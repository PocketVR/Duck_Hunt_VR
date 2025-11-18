/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SCENE
#define UNGINE_SCENE

/*────────────────────────────────────────────────────────────────────────────*/

/*
namespace ungine { struct environment_t {
    color_t color = rl::WHITE;
    ptr_t<shader_t> shader;
    vec3_t sun_direction;
    bool  cast_shadow;
    float sun_energy;
    float fog = 0.0f;
    color_t ambient_light = rl::WHITE;
};}
*/

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace render {

    void emit_scissor( rect_t scissor, function_t<void> callback ) {
         rl::BeginScissorMode( scissor.x, scissor.y, scissor.width, scissor.height );
         callback(); rl::EndScissorMode(); /*-------------------------------------*/
    }

    void emit_render( render_2D_t render, function_t<void> callback ) {
         rl::BeginTextureMode( render ); callback();
         rl::EndTextureMode  (); /*---------------*/
    }

    void emit_shader( shader_t* shader, function_t<void> callback ) {
         if( shader == nullptr ){ return; }
         rl::BeginShaderMode( shader->get() ); callback();
         rl::EndShaderMode  ();
    }

    void emit_blend( uint blend_mode, function_t<void> callback ) {
         rl::BeginBlendMode( blend_mode ); callback();
         rl::EndBlendMode  (); /*-------------------*/
    }

    void emit_2D( camera_2D_t* cam, function_t<void> callback ) {
         if( cam == nullptr ){ return; }
         rl::BeginMode2D( *cam ); callback();
         rl::EndMode2D  (); /*-------------*/
    }

    void emit_3D( camera_3D_t* cam, function_t<void> callback ) {
         if( cam == nullptr ){ return; }
         rl::BeginMode3D( *cam ); callback();
         rl::EndMode3D  (); /*-------------*/
    }

    void emit_vr( vr_t* device, function_t<void> callback ) {
         if( device == nullptr ){ return; }
         rl::BeginVrStereoMode( device->device() ); callback();
         rl::EndVrStereoMode  (); /*-------------------------*/
    }

    void emit( function_t<void> callback ) {
         rl::BeginDrawing(); callback();
         rl::EndDrawing  (); /*-------*/
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node { 
node_t node_render( function_t<void,ref_t<node_t>> clb ) {
return node_t([=]( ref_t<node_t> self ){

    auto rnd   = type::bind( render_t() ); 
    auto tmp   = viewport_t();
    tmp.render = type::bind( rnd );

    self->set_attribute( "type"    , "Render" );
    self->set_attribute( "vr"      , vr_t() );
    self->set_attribute( "viewport", tmp );
    
    self->onDraw([=](){

        auto vpt = self->get_viewport(); if(vpt==nullptr){ return; }
        auto vrr = self->get_attribute<vr_t>( "vr" );
        auto que = self->get_render_queue();

    render::emit_render( vpt->render->get(), [&](){
    if( vpt->mode & visibility::MODE::VISIBILITY_MODE_VR && vrr != nullptr
    ) { rl::BeginVrStereoMode( vrr->device() ); }

        rl::ClearBackground( vpt->background );

        if( !que->event3D.empty() ){
        if( !vpt->camera3D.null() ){
        render::emit_3D( &vpt->camera3D, [&](){

            auto x=que->event3D.first(); while( x!=nullptr ){
            auto y=x->next; x->data.emit(); x=y; }

        }); }}

        if( !que->event2D.empty() ){ 
        if( !vpt->camera2D.null() ){
        render::emit_2D( &vpt->camera2D, [&](){

            auto x=que->event2D.first(); while( x!=nullptr ){
            auto y=x->next; x->data.emit(); x=y; }

        }); } else {

            auto x=que->event2D.first(); while( x!=nullptr ){
            auto y=x->next; x->data.emit(); x=y; }

        }}
        
        if( !que->eventUI.empty() ){

            auto x=que->eventUI.first(); while( x!=nullptr ){
            auto y=x->next; x->data.emit(); x=y; }

        }

    if( vpt->mode & visibility::MODE::VISIBILITY_MODE_VR && vrr != nullptr
    ) { rl::EndVrStereoMode(); } }); }); clb( self ); 

}); }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace node {
node_t node_scene( function_t<void,ref_t<node_t>> clb ) {
return node_render([=]( ref_t<node_t> self ){

    self->onNext([=](){ auto root = self->get_root();

        queue_t<node_t*> collision_list;

        root->child_iterator([=]( node_t* node ){
            if( !node->has_attribute( "visibility" ) ){ return true; }

            auto vis = node->get_attribute<visibility_t>("visibility");
            if ( vis->mode==0x00 || vis->mask==0x00 ){ return false; }
            if ( !node->has_attribute("collision")  ){ return true ; }

            /*---------------------------*/ collision_list.push( node );
        return true; }, true ); auto data = collision_list.data();

        /* FIXME: collision caching system */

        for( auto x:data ) { for( auto y:data ) {
        if ( x == y ) /*----------*/ { continue; }
        if ( x->onCollision.empty() ){ continue; }
        collision::next( *x,*y ); }}
        
    });

    self->onDraw([=](){

        auto vpt = self->get_viewport(); if(vpt==nullptr){ return; }
        auto shd = self->get_attribute<shader_t>( "shader" );
        auto vrr = self->get_attribute<vr_t>    ( "vr" );

    if( shd != nullptr && shd->is_valid() )
      { shd->append_uniform( "texture", *vpt->render ); }

    render::emit( [&](){ if( vpt->render.null() ){ return; }
    if( shd != nullptr && shd->is_valid() )
      { rl::BeginShaderMode( shd->get() ); shd->set_variables(); }
        
        auto txt = vpt->render->get_texture(); rl::ClearBackground( rl::BLANK );
        auto src = rect_t({ 0, 0, type::cast<float>( txt.width ), type::cast<float>(-txt.height ) });
        auto dst = rect_t({ 0, 0, type::cast<float>( txt.width ), type::cast<float>( txt.height ) });

        rl::DrawTexturePro ( txt, src, dst, vec2_t({ 0, 0 }), .0f, rl::WHITE );

    if( shd != nullptr && shd->is_valid() ){ rl::EndShaderMode(); }
    }); }); clb( self );
 
}); }}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
