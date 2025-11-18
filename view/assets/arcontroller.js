function ar_loop( arController, video ) { try {
    
    for( var i in window.marker ){ window.marker.visible=false; }
    arController.process( video ); /*--------------------------*/
    requestAnimationFrame(()=>{ ar_loop( arController, video ); });

} catch(err) { alert( err ); }}

function ar_setup( arController, video, callback ) {

    arController.setPatternDetectionMode( artoolkit.AR_MATRIX_CODE_DETECTION );
    arController.addEventListener( 'getMarker', (ev)=>{
    if( ev.data.marker.idMatrix === -1 ){ return; }
        callback( ev ); /*-----------*/
    }); ar_loop( arController, video );

}

function start_ar_controller( callback ) {

    if( navigator.mediaDevices && navigator.mediaDevices.getUserMedia ) {

        var hint = new Object({
            facingMode: { "ideal": "environment" },
            audio     : false
        });

        var video = document.querySelector("video");

        navigator.mediaDevices.getUserMedia({video:hint}).then((stream)=>{
            video.srcObject  = stream; video.play();
            var arController = new ARController( video, "/assets/camera_para.dat" );
                arController.onload = ()=>{ ar_setup( arController, video, callback ); }
        }).catch((err)=>{
            console.log( err );
            alert( "error while stating camera " );
        });

    } else { alert( "camera not found" ); }

}
