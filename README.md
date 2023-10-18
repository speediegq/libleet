# libleet

C++ Matrix library

## Currently supports

- Connecting to a Matrix instance
- Logging in
- Message (including attachment) downloading
- Message sending
- User listing
- Room listing (including those inside spaces)
- Space listing
- Aliases
- mxc:// uploading and downloading
- Discovery

This library is very much work in progress. Expect things to change,
and as such it is not suitable for use yet. If you wish to help
implement some of the many Matrix features, feel free to do so.

## Dependencies

- cpr
- olm
- nlohmann_json
- C++20

## Design goals

- Easy to use
- Make use of C++ features
- Be modern. libleet uses modern C++ features where appropriate
- Simplify much of the junk so that developers can just focus on
designing their user interface

## Features to implement

See TODO.
