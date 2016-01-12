## Zippie

A zip archive handler tool, attempting to achieve API compatibility with Pythons own zipfile module.

    python setup.py install

    >>> import zippie
    >>> zf = zippie.PieZipFile("/path/to/file.zip")

## Done
 - [X] [DONE] scopedstreambuf that limits the available scope to the underlying filestream being used
 - [X] [DONE] read central directory with zip64 support
 - [X] [DONE] read local file headers with support for data descriptor records
 - [X] [DONE] seek within zip archive members (that one pythons version doesn't have, but with encryption and/or compression, it will get tricky and undefined atm)
 - [X] [DONE] api to perform extraction of zip members
 - [X] [DONE] streambuf that performs crc32 calculation while reading

## In Progress
 - [ ] [IN PROGRESS] streambuf that performs decryption while reading
 - [ ] [IN PROGRESS] expose all header fields through the memberinfo api

## TODO
 - [ ] [TODO] fix screwups
 - [ ] [TODO] write / create zipfiles
 - [ ] [TODO] make code utilize C++11 features (especially move semantics for istream is needed)
 - [ ] [TODO] implement compression support using zlib...maybe
