# muconvert: A thin C and Rust wrapper over `mutool convert` that extracts text from pdf

## Introduction

`mutool convert` from https://mupdf.com/ seems to always write extracted text to a file.
So I adapt its code a bit to read pdf text into an `unsigned char` buffer.
I further add a thin Rust wrapper so that the same can be done from Rust side.
All C and Rust sum up to less than 300 lines of codes.

## Why another pdf extractor

I'm aware of a number of awesome Rust pdf text extractors like [pdf-extract](https://github.com/jrmuizel/pdf-extract).
There's also a rust binding of `mupdf` [here](https://github.com/messense/mupdf-rs).
However, these libraries appear too large for my use case.
That's the main reason why I start this project.

## Build C

Please refer to the `Makefile`.

## Build Rust binding

Not a perfect procedure, but basically:

1. Install `mupdf` (e.g., on macOS, `brew install mupdf`)
2. Adjust the `Makefile` to your platform (mine is macOS). You may also need to adjust [`build.rs`](https://doc.rust-lang.org/cargo/reference/build-scripts.html).
3. Run `make dylib`. This should produce `libmuconvert.dylib` on macOS.
4. Run `cargo build`.

## Rust usage

```rust
let filename = "hello.pdf";
// Assign a large enough buffer for the pdf (NOTE below),
// and you'll be fine.
let buf: Vec<u8> = vec![0; 103977368];
let text = muconvert_rust::pdftotext(filename, false, true, buf)?;
```

NOTE: I know this is a bit awkward.
Currently, in case of a buffer too small error, here is a possible solution:

```rust
use muconvert_rust::{Error, pdftotext};

let filename = "hello.pdf";
// A very small buffer.
let buf: Vec<u8> = vec![0; 100];
match pdftotext(filename, false, true, buf) {
    // Do something with the extracted text.
    Ok(text) => (),
    // In case of the buffer too small error,
    Err(Error::BufferTooSmall(len, mut buf)) => {
        // Extend the buffer.
        buf.resize(len, 0);
        // Retry, and do something with the text.
        let text = pdftotext(filename, false, true, buf).unwrap();
    }
    // Handle other errors.
    _ => (),
}
```

But I haven't yet found a way to keep the wrapper simple while avoiding such issue.
Suggestions are welcome!
