#include <nodepp/nodepp.h>
#include <ungine/ungine.h>
#include <nodepp/http.h>
#include <nodepp/json.h>
#include <nodepp/bind.h>

using namespace nodepp;

#include "../controller/script/utils.cpp"
#include "../controller/scene/scene_0.cpp"

void onMain() {

    auto width  = EM_ASM_INT({ return window.screen.width;  });
    auto height = EM_ASM_INT({ return window.screen.height; });

    EM_ASM({ (()=>{

        document.querySelector( "button" ).addEventListener( "click",()=>{
            window.document.querySelector("canvas").requestFullscreen();
        });

        window.addEventListener( "resize", ()=>{ engine.refresh(); });
        window.sensor = new Object({ x:"0", y:"0", z:"0", w:"0" });

    try {

        const sensor = new AbsoluteOrientationSensor({ frequency: 60 });
        sensor.addEventListener( "reading", ()=>{
            window.sensor.x = sensor.quaternion[0].toFixed(6);
            window.sensor.y = sensor.quaternion[1].toFixed(6);
            window.sensor.z = sensor.quaternion[2].toFixed(6);
            window.sensor.w = sensor.quaternion[3].toFixed(6);
        }); sensor.start();

    } catch(err) { alert( "gyroscope not found" ); } 

        setTimeout(()=>{ start_ar_controller((ev)=>{
            engine.marker( 
                ev.data.marker.idMatrix, 
                ev.data.matrix 
            );
        }); },1000);

    })(); }); srand( process::now() );

    ungine::engine::start( width, height, "GAME" );
    ungine::engine::set_fps( 60 );
    ungine::scene ::scene_0();

}
