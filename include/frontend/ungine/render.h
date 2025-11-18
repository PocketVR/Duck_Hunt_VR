/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_RENDER
#define UNGINE_RENDER

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class render_t : public global_t {
protected:

    struct NODE { rl::RenderTexture txt; char filter=-1; }; ptr_t<NODE> obj;

public:

    /*─······································································─*/

    virtual ~render_t() noexcept { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    render_t( rl::RenderTexture texture ) noexcept : global_t(), obj( new NODE() ){
        obj->txt = texture;
    }

    render_t( int width, int height ) noexcept : global_t(), obj( new NODE() ){
        set_size( width, height );
    }
    
    render_t() noexcept : global_t(), obj( new NODE() ){ 
        set_size( rl::GetRenderWidth(), rl::GetRenderHeight() );
    }

    /*─······································································─*/

    rl::RenderTexture* operator->() const noexcept { return &get(); }

    rl::RenderTexture& get() const noexcept { return obj->txt; }

    void set_filter( uint filter ) const noexcept {
         if( obj->filter==filter ){ return; }
         rl::SetTextureFilter( obj->txt.depth  , filter );
         rl::SetTextureFilter( obj->txt.texture, filter );
         obj->filter = filter;
    }

    bool is_valid() const noexcept { 
        if( obj->txt.texture.width == 0 ){ return 0; }
        return rl::IsRenderTextureValid( obj->txt ); 
    }

    /*─······································································─*/

    rl::Vector2 size() const noexcept {
        if( !is_valid() ){ return rl::Vector2({ 0, 0 }); }
        /*--------------*/ return { 
            type::cast<float>( obj->txt.texture.width  ), 
            type::cast<float>( obj->txt.texture.height ) 
        };
    }

    rl::Texture& get_texture() const noexcept { return obj->txt.texture; }

    rl::Texture& get_depth  () const noexcept { return obj->txt.depth; }

    /*─······································································─*/

    void draw( transform_2D_t pos, rect_t src ) const noexcept {
        auto origin = pos.translate.scale / 2; rl::DrawTexturePro( 
            obj->txt.texture, src, rect_t({
                pos.translate.position.x, pos.translate.position.y,
                pos.translate.scale.x,    pos.translate.scale.y
            }), origin, pos.translate.rotation * RAD2DEG, rl::WHITE 
        );
    }

    void draw( transform_2D_t pos ) const noexcept {
        auto origin = pos.translate.scale / 2; rl::DrawTexturePro( 
            obj->txt.texture, rect_t({
                0, 0, type::cast<float>( obj->txt.texture.width  ), 
                /*-*/ type::cast<float>( obj->txt.texture.height )
            }), rect_t({
                pos.translate.position.x, pos.translate.position.y,
                pos.translate.scale.x,    pos.translate.scale.y
            }), origin, pos.translate.rotation * RAD2DEG, rl::WHITE 
        );
    }

    /*─······································································─*/

    void emit( function_t<void> cb ) const noexcept {
         rl::BeginTextureMode( obj->txt ); cb();
         rl::EndTextureMode  (); /*-----------*/
    }

    /*─······································································─*/

    vec2_t get_size() const noexcept { return vec2_t({ 
        type::cast<float>( obj->txt.texture.width  ), 
        type::cast<float>( obj->txt.texture.height ) 
    }); }

    void set_size( int width, int height ) const noexcept {
        if( obj->txt.texture.width==width && obj->txt.texture.height==height )
          { return; } free(); obj->txt = rl::LoadRenderTexture( width,height );
          set_filter( rl::TEXTURE_FILTER_BILINEAR );
    }

    /*─······································································─*/

    void free() const noexcept {
         if( !is_valid() ){ return; } 
         rl::UnloadRenderTexture( obj->txt ); obj->filter=-1;
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
