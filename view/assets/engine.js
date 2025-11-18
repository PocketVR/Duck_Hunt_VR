window.addEventListener( "load", ()=>{

document.querySelector("[start-game]").addEventListener( "click", ()=>{
document.querySelector("[start-game]").style["display"] = "none";
document.querySelector("canvas")      .hidden = false;

    Engine().then(( ev )=>{ 
        
        ev.canvas     = document.querySelector("canvas");
        window.engine = ev;

    }).catch(( err )=>{
        console.log( err );
        alert( "error while launching the game" );
    });

});

});