<center>
    <img alt="Kal Logo" src="https://raw.githubusercontent.com/KILLinefficiency/KalWebsite/refs/heads/master/static/logo.svg">
    <h1>Kal</h1>
</center>

<hr />

Kal is an interpreted programming that aims to combine different programming paradigms together to provide best developer experience possible.

Striking features:

* **Lightweight:** Fast with extremely low memory usage. Kal takes care of garbage collection.
* **Dynamic:** No boundaries on types. Reassign a variable to a value of a completely different type.
* **Procedural:** Abstract your code into reusable functions and split them across files.
* **Batteries Included:** There is no separate standard library. All features are interpreter intrinsic.
* **Package Management:** Manage third party / reusable packages centrally and share them across projects.
* **Embeddable:** Embed Kal into existing C++, Python and JavaScript applications seamlessly via official 
bindings.
* **Independent:** Kal is written completely from ground up without utilizing any third party library.

<hr />

## Installation

Installing Kal is straightforward, clone the repository...

```bash
$ git clone --depth=1 https://www.github.com/KILLinefficiency/Kal
```

...and install.

```bash
$ cd Kal
$ ./build.sh install
```

<hr />

## Usage

Write a Kal program `sort.kal`...
```
fn sort -> data {
    var i = 0.

    loop i < $(len data) {
        var j = i.

        loop j < $(len data) {
            if data[i] > data[j] {
                var [data[i], data[j]]
                    = [data[j], data[i]].
            }
            j = j + 1.
        }

        i = i + 1.
    }

    <- data.
}

:sort [3, 1, 5, 4, 2] -> data.
stdout "Sorted:  " data "\n".
```

...and run it.

```bash
$ kal sort.kal
```

<hr />

## The Human Touch

Every line of the Kal codebase is handcrafted with no AI assistance whatsoever. The codebase aims to stay that way. :)

<hr />

## Related Repositories

* [Kal](https://github.com/KILLinefficiency/Kal) - Core Kal Interpreter.
* [PyKal](https://github.com/KILLinefficiency/PyKal) - Python Bindings for Embeddable Kal.
* [JSKal](https://github.com/KILLinefficiency/JSKal) - JavaScript Bindings for Embeddable Kal.
* [Kal Website](https://github.com/KILLinefficiency/KalWebsite) - Offical Kal Website.