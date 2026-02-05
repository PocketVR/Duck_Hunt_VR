window.addEventListener( "load", ()=>{

    var Module = {
        preRun: [], postRun: [],
        canvas: document.getElementById('canvas')
    };

    Engine( Module ).then(( ev )=>{ 

        ev.canvas     = document.querySelector("canvas");
        window.engine = ev;

    }).catch(( err )=>{
        console.log( err );
        alert( "error while launching the game" );
    });

});