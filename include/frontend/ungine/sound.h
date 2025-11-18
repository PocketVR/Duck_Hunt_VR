/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_SOUND
#define UNGINE_SOUND

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { struct sound_t {
protected:

    struct NODE { rl::Sound ctx; }; ptr_t<NODE> obj;

public:

    template< class T >
    sound_t( T& file, string_t ext ) noexcept : obj( new NODE() ){
        auto raw = stream::await( file ); /*-------------------------------*/
        auto wav = rl::LoadWaveFromMemory( ext.get(), raw.get(), raw.size() );
        obj->ctx = rl::LoadSoundFromWave( wav ); rl::UnloadWave( wav );
    }

    sound_t( rl::Sound ctx ) noexcept : obj( new NODE() ) { obj->ctx = ctx; }

    sound_t( string_t path ) noexcept : obj( new NODE() ) {
        obj->ctx = rl::LoadSound( path.get() );
    }

    /*----*/ sound_t() noexcept : obj( new NODE() ) {}
    virtual ~sound_t() noexcept { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    void set_volume( float val ) const noexcept { rl::SetSoundVolume( obj->ctx, val ); }

    void set_pitch( float val ) const noexcept { rl::SetSoundPitch( obj->ctx, val ); }

    void set_pan( float val ) const noexcept { rl::SetSoundPan( obj->ctx, val ); }

    /*─······································································─*/

    bool is_playing() const noexcept { return rl::IsSoundPlaying( obj->ctx ); }

    /*─······································································─*/

    void play  () const noexcept { rl::PlaySound  ( obj->ctx ); }

    void pause () const noexcept { rl::PauseSound ( obj->ctx ); }

    void stop  () const noexcept { rl::StopSound  ( obj->ctx ); }

    void resume() const noexcept { rl::ResumeSound( obj->ctx ); }

    /*─······································································─*/

    rl::Sound* operator->() const noexcept { return &get(); }

    rl::Sound& get() const noexcept { return obj->ctx; }

    bool is_valid() const noexcept { 
        if( obj->ctx.frameCount == 0 ){ return false; }
        return rl::IsSoundValid( obj->ctx ); 
    }

    /*─······································································─*/

    void free() const noexcept {
         if( !is_valid() ){ return; } rl::UnloadSound( obj->ctx );
    }

}; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class music_t {
protected:

    struct NODE { rl::Music ctx; bool shot=false; }; ptr_t<NODE> obj;

public:

    template< class T >
    music_t( T& file, string_t ext ) noexcept : obj( new NODE() ){
        auto data = stream::await( file ); /*----------------------------------------*/
        obj->ctx  = rl::LoadMusicStreamFromMemory( ext.get(), data.get(), data.size() );
    }

    music_t( rl::Music ctx ) noexcept : obj( new NODE() ) { obj->ctx = ctx; }

    music_t( string_t path ) noexcept : obj( new NODE() ) {
        obj->ctx = rl::LoadMusicStream( path.get() );
    }

    /*----*/ music_t() noexcept : obj( new NODE() ) {}
    virtual ~music_t() noexcept { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    void set_volume( float val ) const noexcept { rl::SetMusicVolume( obj->ctx, val ); }

    void set_pitch( float val ) const noexcept { rl::SetMusicPitch( obj->ctx, val ); }

    void set_pan( float val ) const noexcept { rl::SetMusicPan( obj->ctx, val ); }

    /*─······································································─*/

    bool is_playing() const noexcept { return rl::IsMusicStreamPlaying( obj->ctx ); }

    /*─······································································─*/

    void  set ( float pos ) const noexcept { rl::SeekMusicStream( obj->ctx, pos ); }

    float size() const noexcept { return rl::GetMusicTimeLength( obj->ctx ); }
    
    float seek() const noexcept { return rl::GetMusicTimePlayed( obj->ctx ); }

    /*─······································································─*/

    void stop  () const noexcept { rl::StopMusicStream  ( obj->ctx ); obj->shot=false; }

    void shot  () const noexcept { resume(); play(); obj->shot=true ; }

    void play  () const noexcept { rl::PlayMusicStream  ( obj->ctx ); }

    void pause () const noexcept { rl::PauseMusicStream ( obj->ctx ); }

    void resume() const noexcept { rl::ResumeMusicStream( obj->ctx ); }

    void update() const noexcept { rl::UpdateMusicStream( obj->ctx ); }

    /*─······································································─*/

    void next() const noexcept { 
    //  console::log( obj->shot, fabsf(seek()-size()), is_playing() );
        if( obj->shot && fabsf(seek()-size())<0.1f && is_playing() )
          { stop(); set(0.0f); return; } update();
    }

    /*─······································································─*/

    rl::Music* operator->() const noexcept { return &get(); }

    rl::Music& get() const noexcept { return obj->ctx; }

    bool is_valid() const noexcept { 
        if( obj->ctx.frameCount == 0 ){ return false; }
        return rl::IsMusicValid( obj->ctx ); 
    }

    /*─······································································─*/

    void free() const noexcept {
         if( !is_valid() ){ return; } rl::UnloadMusicStream( obj->ctx );
    }

}; }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/