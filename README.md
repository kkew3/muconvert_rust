# muconvert: A thin C and Rust wrappers over `mutool convert` that extract text from pdf

## Introduction

`mutool convert` from https://mupdf.com/ seems to always write extracted text to a file.
So I adapt its code a bit to read pdf text into an `unsigned char` buffer.
I further add a thin Rust wrapper so that the same can be done from Rust side.
All C and Rust sum up to less than 300 lines of codes.

## Why another pdf extractor

I'm aware of a number of awesome Rust pdf text extractor like [pdf-extract](https://github.com/jrmuizel/pdf-extract).
There's also a rust binding of `mupdf` [here](https://github.com/messense/mupdf-rs).
However, these libraries appears too large for my use case.
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
// Assign a large enough buffer for the pdf (NOTE below).
let buf: Vec<u8> = vec![0; 103977368];
// If you capture the error, and if it's due to buffer too small, it will tell
// you how many bytes the buffer needs to have, and you may try again with the
// new buffer ...
let text = muconvert_rust::pdftotext(filename, false, true, buf)?;
```

NOTE: I know this is a bit awkward.
But I haven't yet found a way to keep the wrapper simple while circumvent such issue.
Suggestions are welcome!
