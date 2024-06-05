fn main() {
    let mupdf_version = "1.23.11";
    println!(
        "cargo::rustc-link-search=/usr/local/Cellar/mupdf/{}/lib",
        mupdf_version
    );
    println!("cargo::rustc-link-lib=mupdf");
    println!("cargo::rustc-link-lib=mupdf-third");
    println!("cargo::rustc-link-search=.");
    println!("cargo::rustc-link-lib=muconvert");
}
