var app = document.getElementById("tw");

var typewriter = new Typewriter(app, {
    loop: true
});

typewriter.typeString('Fast')
    .pauseFor(1500)
    .deleteAll()
    .typeString('Minimal')
    .pauseFor(1500)
    .deleteAll()
    .typeString('Powerful')
    .pauseFor(1500)
    .deleteAll()
    .typeString('Customizable')
    .pauseFor(1500)
    .deleteAll()
    .typeString('Extensible')
    .pauseFor(1500)
    .typeString('<b>Navi</b>')
    .pauseFor(1500)
    .start();

function get_started() {
    window.location.href = "./installation.html";
}
