
var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('Fast')
    .pauseFor(2500)
    .deleteAll()
    .typeString('Powerful')
    .pauseFor(2500)
    .deleteAll()
    .typeString('Extensible')
    .pauseFor(2500)
    .deleteAll()
    .typeString('Customizable')
    .pauseFor(2500)
    .deleteAll()
    .start();