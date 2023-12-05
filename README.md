# libleet

C++ Matrix client library which aims to support most features in the Matrix
specification, as well as Element features for a great user experience.

**This library is very much work in progress.**
Expect things to change, and as such it is not suitable for use
yet. If you wish to help implement some of the many Matrix
features, feel free to do so in the form of a pull request.

libleet is the library that powers the backend for the
work-in-progress Matrix client [stellar](https://git.speedie.site/speedie/stellar).

## Currently supports

- Connecting to a Matrix instance
- Logging in
- Message (including attachment) downloading
- Message sending
- Room creation
- Room upgrades
- Room listing (including those inside spaces)
- Space listing
- User listing
- Aliases
- Attachment uploading and downloading
- Discovery
- Refresh tokens
- Read marker
- User presence
- End to end encryption
  - Can send encrypted text messages.
  - Currently **cannot** send encrypted attachments, read encrypted messages,
  verify interactively or use key backups.
- Reporting content
- VOIP
  - Can retrieve TURN server credentials. Not much thought has been put into
  VOIP support, because I am not a big fan of how it is implemented in Matrix.
- And more trivial features that I don't think are worth listing here.

## Not yet supported

- Managing rooms/spaces
- Creating spaces
- Storing custom data
- Device management
- Threads
- Third-party/Phone number/SSO authentication
- Various end-to-end encryption features
- Push notification endpoints

## Dependencies

- cpr
- nlohmann\_json
- C++20 [compiler](https://en.cppreference.com/w/cpp/compiler_support/20)
- olm
  - For end to end encryption, -DLEET\_NO\_ENCRYPTION to disable
- openssl
  - For end to end encryption, -DLEET\_NO\_ENCRYPTION to disable

To install these dependencies on **Debian**:

- `apt install meson nlohmann-json3-dev libolm-dev libssl-dev`
- Note that libolm is not available from standard Debian bookworm repositories.
- libcpr must be compiled from source on Debian,
[see instructions here](https://github.com/libcpr/cpr).

If you're too lazy to compile these libraries yourself (I can't blame you),
you can simply rely on Meson. Meson will automatically download and build
all of the dependencies if they are not present on your system,
though if you're running Debian 12 it should be noted that you MUST
update your Meson version, otherwise it will fail to get the dependencies
where CMake is used upstream. If you use SID, this is not a problem you have
to deal with.

## Compiling and installing

If you use the meson build system, you can compile
libleet like this:

- `meson setup build --prefix=/usr --reconfigure`
- `cd build`
- `meson install`

To generate documentation: `cd docs; doxygen; cd ..`

If you're compiling with Visual Studio, you can use the
solution. I don't use Windows so I don't really know if
it works very well. Please note that with this approach
you must compile olm separately for Windows.

## Design goals

- Easy to use
- Make use of C++ features
- Support Element as well as possible
- Be modern. libleet uses modern C++ features where appropriate
- Simplify much of the junk so that developers can just focus on
designing their user interface

## Features to implement

See the Issue tracker, as well as 'Not yet supported'.

## Examples

You can find examples in the examples subdirectory.

In addition,
[stellar-backend](https://git.speedie.site/speedie/stellar-backend)
is a web API which may also *serve* (get it?) as an example
for how to write a program to interface with Matrix using libleet.

## License

The project is licensed under the GNU Affero General
Public License version 3.0, which means it is
free software as defined by the Free Software Foundation.

Copyright (c) 2023 speedie (https://speedie.site)
