use std::ffi::{c_char, c_int, CString};

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

#[derive(Debug)]
pub enum Error {
    OutOfBuffer,
    OtherFailure,
}

impl From<i32> for Error {
    fn from(value: i32) -> Self {
        if value == 2 {
            Self::OutOfBuffer
        } else {
            Self::OtherFailure
        }
    }
}

pub fn pdftotext(
    filename: &str,
    dehyphenate: bool,
    buf: &mut Vec<u8>,
) -> Result<usize, Error> {
    let filename: *const c_char = CString::new(filename).unwrap().into_raw();
    let dehyphenate: c_int = if dehyphenate { 1 } else { 0 };
    let data = buf.as_mut_ptr();
    let mut len = buf.len();
    let len_ptr: *mut usize = &mut len;
    let retcode = unsafe {
        muconvert_internal::pdftotext(filename, dehyphenate, data, len_ptr)
    };
    if retcode == 0 {
        Ok(len)
    } else {
        Err(Error::from(retcode))
    }
}
