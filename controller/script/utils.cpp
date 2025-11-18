#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

object_t marker_list;

/*────────────────────────────────────────────────────────────────────────────*/

void marker ( const int& id, const EM_VAL& data ){

    auto matrix = ungine::mat_t({
        data[0].as<float>(), data[4].as<float>(), data[8] .as<float>(), data[12].as<float>(),
        data[1].as<float>(), data[5].as<float>(), data[9] .as<float>(), data[13].as<float>(),
        data[2].as<float>(), data[6].as<float>(), data[10].as<float>(), data[14].as<float>(),
        data[3].as<float>(), data[7].as<float>(), data[11].as<float>(), data[15].as<float>()
    });

    if( !marker_list.has( string::to_string(id) ) ){
        marker_list[string::to_string(id)] = object_t({
            { "timestmp" , process::now() + TIME_SECONDS(1) },
            { "data"     , matrix }
        });
    } else { auto marker = marker_list[string::to_string(id)];
        marker["timestmp"] = process::now()+ TIME_SECONDS(1);
        marker["data"]     = matrix;
    }

}

/*────────────────────────────────────────────────────────────────────────────*/

void resize ( int width, int height ){ ungine::window::resize({ width, height }); }

void refresh( /*-----------------*/ ){
    auto width  = EM_ASM_INT({ return window.screen.width ; });
    auto height = EM_ASM_INT({ return window.screen.height; });
    resize( width, height ); /*-----------------------------*/
}

/*────────────────────────────────────────────────────────────────────────────*/

BIND( Engine ){
BIND_ADD( "marker" , &marker  )
BIND_ADD( "resize" , &resize  )
BIND_ADD( "refresh", &refresh )}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { vec4_t get_sensor_data() { try {

    char res[UNBFF_SIZE]; auto size = EM_ASM_INT({
        var data = JSON.stringify( window.sensor );
        stringToUTF8( data, $0, $1 ); return data.length;
    }, res, UNBFF_SIZE );

    auto data = json::parse( string_t( res, size ) );

    return vec4_t({
        string::to_float( data["x"].as<string_t>() ),
        string::to_float( data["y"].as<string_t>() ),
        string::to_float( data["z"].as<string_t>() ),
        string::to_float( data["w"].as<string_t>() )
    });

} catch( except_t ) { return vec4_t({ 0, 0, 0, 0 }); }} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { map_t<string_t,function_t<model_t>>& get_model_assets() { 
    static map_t<string_t,function_t<model_t>> out({

    { "bird", [=]() -> model_t& { switch( rand() % 3 ){

        case 0 : do { static model_t model;
            if( !model.is_valid() ){ model = model_t( "./controller/assets/bird_1.glb" ); }
        return model; } while(0); break;

        case 1 : do { static model_t model;
            if( !model.is_valid() ){ model = model_t( "./controller/assets/bird_2.glb" ); }
        return model; } while(0); break;

        default: do { static model_t model;
            if( !model.is_valid() ){ model = model_t( "./controller/assets/bird_3.glb" ); }
        return model; } while(0); break;

    } } },

    { "scene", [=]() -> model_t& { static model_t model;
        if( !model.is_valid() ){ model = model_t( "./controller/assets/scene_1.glb" ); }
        return model;
    } },

    { "rabbit", [=]() -> model_t& { static model_t model;
        if( !model.is_valid() ){ model = model_t( "./controller/assets/rabbit_1.glb" ); }
        return model;
    } },

    { "pistol", [=]() -> model_t& { static model_t model;
        if( !model.is_valid() ){ model = model_t( "./controller/assets/pistol.glb" ); }
        return model;
    } },

}); return out; }}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { map_t<string_t,function_t<music_t>>& get_music_assets() { 
    static map_t<string_t,function_t<music_t>> out({

    { "scene", [=]() -> music_t& { static music_t model;
        if( !model.is_valid() ){ model = music_t( "./controller/assets/scene.mp3" ); }
        return model;
    } },

    { "pistol", [=]()-> music_t& { static music_t model;
        if( !model.is_valid() ){ model = music_t( "./controller/assets/pistol.mp3" ); }
        return model;
    } },

    { "level", [=]()-> music_t& { static music_t model;
        if( !model.is_valid() ){ model = music_t( "./controller/assets/level.mp3" ); }
        return model;
    } },

}); return out; }}

/*────────────────────────────────────────────────────────────────────────────*/