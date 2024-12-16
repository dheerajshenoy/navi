var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('')
    .pauseFor(100)
    .deleteAll()
    .typeString('Minimal')
    .pauseFor(100)
    .deleteAll()
    .typeString('Fast')
    .pauseFor(100)
    .deleteAll()
    .typeString('Powerful')
    .pauseFor(100)
    .deleteAll()
    .typeString('Extensible')
    .pauseFor(100)
    .start();
