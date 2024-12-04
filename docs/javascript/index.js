var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('Fast')
    .pauseFor(500)
    .deleteAll()
    .typeString('Minimal')
    .pauseFor(500)
    .deleteAll()
    .typeString('Powerful')
    .pauseFor(500)
    .deleteAll()
    .typeString('Customizable')
    .pauseFor(500)
    .deleteAll()
    .typeString('Extensible')
    .pauseFor(500)
    .typeString('<b>Navi</b>')
    .pauseFor(500)
    .deleteAll()
    .start();

function get_started() {
    window.location.href = "./installation.html";
}
