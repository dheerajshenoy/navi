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

function get_started() {
    window.location.href = "./installation.html";
}

function openNavMenu() {
    var x = document.getElementById("topnav");
    if (x.className === "topnav") {
      x.className += " responsive";
    } else {
      x.className = "topnav";
    }
}