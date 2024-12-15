var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('')
    .pauseFor(200)
    .deleteAll()
    .typeString('Minimal')
    .pauseFor(200)
    .deleteAll()
    .typeString('Powerful')
    .pauseFor(200)
    .deleteAll()
    .typeString('Customizable')
    .pauseFor(200)
    .deleteAll()
    .typeString('Extensible')
    .pauseFor(200)
    .start();
