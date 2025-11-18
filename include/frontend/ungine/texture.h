/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_TEXTURE
#define UNGINE_TEXTURE

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class texture_t : public global_t {
protected:

    struct NODE { rl::Texture2D txt; char filter=-1; }; ptr_t<NODE> obj;

public:

    /*----*/ texture_t() noexcept : global_t(), obj( new NODE() ){ /*---*/ }
    virtual ~texture_t() noexcept { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    texture_t( rl::Texture2D texture ) noexcept : global_t(), obj( new NODE() ) { obj->txt = texture; }

    template< class T >
    texture_t( T& file, string_t ext ) noexcept : global_t(), obj( new NODE() ) {
        auto data = stream::await( file ); /*--------------------------------------------*/
        auto img  = rl::LoadImageFromMemory( ext.get(), (uchar*) data.get(), data.size() );
        obj->txt  = rl::LoadTextureFromImage( img ); rl::UnloadImage( img );
        set_filter( rl::TEXTURE_FILTER_BILINEAR );
    }

    texture_t( rl::Image image ) noexcept : global_t(), obj( new NODE() ) {
        obj->txt = rl::LoadTextureFromImage( image );
        set_filter( rl::TEXTURE_FILTER_BILINEAR );
    }

    texture_t( string_t path ) noexcept : global_t(), obj( new NODE() ) {
        obj->txt = rl::LoadTexture( path.get() );
        set_filter( rl::TEXTURE_FILTER_BILINEAR );
    }


    /*─······································································─*/

    rl::Vector2 size() const noexcept {
        if( !is_valid() ){ return rl::Vector2({ 0, 0 }); }
        /*--------------*/ return { 
            type::cast<float>( obj->txt.width  ), 
            type::cast<float>( obj->txt.height )
        };
    }

    void set_filter( uint filter ) const noexcept {
         if( obj->filter==filter ){ return; }
         rl::SetTextureFilter( obj->txt, filter );
         obj->filter = filter;
    }

    /*─······································································─*/

    rl::Texture* operator->() const noexcept { return &get(); }

    rl::Texture& get() const noexcept { return obj->txt; }

    bool is_valid() const noexcept { 
        if( obj->txt.width == 0 ){ return false; }
        return rl::IsTextureValid( obj->txt ); 
    }

    /*─······································································─*/

    void draw( transform_2D_t pos, rect_t src ) const noexcept {
        auto origin = pos.translate.scale / 2; rl::DrawTexturePro( obj->txt, src, rect_t({
                pos.translate.position.x, pos.translate.position.y,
                pos.translate.scale.x,    pos.translate.scale.y
            }), origin, pos.translate.rotation * RAD2DEG, rl::WHITE 
        );
    }

    void draw( transform_2D_t pos ) const noexcept {
        auto origin = pos.translate.scale / 2; rl::DrawTexturePro( obj->txt, rect_t({
                0, 0, type::cast<float>( obj->txt.width  ), 
                /*-*/ type::cast<float>( obj->txt.height )
            }), rect_t({
                pos.translate.position.x, pos.translate.position.y,
                pos.translate.scale.x,    pos.translate.scale.y
            }), origin, pos.translate.rotation * RAD2DEG, rl::WHITE 
        );
    }

    /*─······································································─*/

    void draw( rect_t pos, rect_t src, float angle=0 ) const noexcept {
        auto origin = vec2_t({ pos.width, pos.height }) / 2; 
        rl::DrawTexturePro( obj->txt, src, rect_t({
                pos.x, pos.y, pos.width, pos.height
            }), origin, angle*RAD2DEG, rl::WHITE 
        );
    }

    void draw( rect_t pos, float angle=0 ) const noexcept {
        auto origin = vec2_t({ pos.width, pos.height }) / 2;
        rl::DrawTexturePro( obj->txt, rect_t({
                0, 0, type::cast<float>( obj->txt.width  ), 
                /*-*/ type::cast<float>( obj->txt.height )
            }), rect_t({
                pos.x    , pos.y,
                pos.width, pos.height
            }), origin, angle*RAD2DEG, rl::WHITE 
        );
    }

    /*─······································································─*/

    void free() const noexcept {
         if( !is_valid() ){ return; } rl::UnloadTexture( obj->txt );
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
