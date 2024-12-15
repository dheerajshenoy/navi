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

function colorPick() {
    var element = document.getElementById("colorPicker").jscolor.show();
}

function colorUpdate(picker) {
    var color = picker.toHEXString();
    var accent = document.querySelector(":root");
    accent.style.setProperty("--accent", color);
    localStorage.setItem("accentColor", color);
}

function loadAccentColor() {
    var savedColor = localStorage.getItem("accentColor");

    if (savedColor) {
        var accent = document.querySelector(":root");
        accent.style.setProperty("--accent", savedColor);
        document.getElementById("colorPicker").value = savedColor;
    }
}

// Initialize color picker and set default color
document.addEventListener("DOMContentLoaded", () => {
    loadAccentColor();
    document
        .getElementById("colorPicker")
        .addEventListener("input", function () {
            colorUpdate(this);
        });
});
