window.addEventListener( "load", ()=>{

    Engine().then(( ev )=>{ 
        
        ev.canvas     = document.querySelector("canvas");
        window.engine = ev;

    }).catch(( err )=>{
        console.log( err );
        alert( "error while launching the game" );
    });

});