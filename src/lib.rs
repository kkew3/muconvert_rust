//! Example usage:
//!
//! ```ignore
//! let filename = "hello.pdf";
//! // Assign a large enough buffer for the pdf.
//! let buf: Vec<u8> = vec![0; 103977368];
//! let text = muconvert_rust::pdftotext(filename, false, true, buf)?;
//! ```

use std::ffi::{c_char, c_int, CString};
use std::string::FromUtf8Error;

mod muconvert_internal {
    extern "C" {
        pub fn pdftotext(
            filename: *const ::std::os::raw::c_char,
            dehyphenate: ::std::os::raw::c_int,
            data: *mut ::std::os::raw::c_uchar,
            cap: *mut usize,
        ) -> ::std::os::raw::c_int;
    }
}

/// Possible errors of `pdftotext`.
#[derive(Debug)]
pub enum Error {
    /// The provided buffer is too small. Return the required minimum length
    /// of the buffer, and the passed in buffer.
    BufferTooSmall(usize, Vec<u8>),
    /// Caused by several reasons including IO error, incorrect password, etc.
    OtherFailure,
    /// Utf-8 decoding error.
    Utf8(FromUtf8Error),
    /// This shouldn't happen normally.
    UnexpectedCode(i32),
}

const RETCODE_OUT_OF_BUFFER: i32 = 2;
const RETCODE_OTHER_FAILURE: i32 = 1;
const RETCODE_SUCCESS: i32 = 0;

/// Extract text from pdf file backed by mupdf.
///
/// Arguments:
///   filename     -- the path to the pdf file
///   dehyphenate  -- `true` to enable `dehyphenate` option in mupdf
///   lossy        -- `true` to retry lossy utf-8 decoding on decoding error
///   buf          -- buffer to receive mupdf's output
///
/// Return:
///   Ok(text)     -- the extracted text
///   Err(err)     -- see doc of `Error`
pub fn pdftotext<P: AsRef<str>>(
    filename: P,
    dehyphenate: bool,
    lossy: bool,
    mut buf: Vec<u8>,
) -> Result<String, Error> {
    let filename: *const c_char =
        CString::new(filename.as_ref()).unwrap().into_raw();
    let dehyphenate: c_int = if dehyphenate { 1 } else { 0 };
    let data = buf.as_mut_ptr();
    let mut len = buf.len();
    let len_ptr: *mut usize = &mut len;
    let retcode = unsafe {
        muconvert_internal::pdftotext(filename, dehyphenate, data, len_ptr)
    };
    if retcode == RETCODE_SUCCESS {
        buf.truncate(len);
        match String::from_utf8(buf) {
            Ok(s) => Ok(s),
            Err(utf8_err) => {
                if lossy {
                    Ok(String::from_utf8_lossy(utf8_err.as_bytes())
                        .into_owned())
                } else {
                    Err(Error::Utf8(utf8_err))
                }
            }
        }
    } else if retcode == RETCODE_OTHER_FAILURE {
        Err(Error::OtherFailure)
    } else if retcode == RETCODE_OUT_OF_BUFFER {
        Err(Error::BufferTooSmall(len + 1, buf))
    } else {
        Err(Error::UnexpectedCode(retcode))
    }
}
