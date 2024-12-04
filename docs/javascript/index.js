var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('Fast')
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
    .deleteAll()
    .typeString('<b>Navi</b>')
    .pauseFor(200)
    .deleteAll()
    .start();

function get_started() {
    window.location.href = "./installation.html";
}
