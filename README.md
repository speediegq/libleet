# libleet

C++ Matrix client library which aims to support most features in the Matrix
specification, as well as Element features for a great user experience.

**This library is very much work in progress.**
Expect things to change, and as such it is not suitable for use
yet unless you feel like rewriting your code at a later date.
If you wish to help implement some of the many Matrix
features, feel free to do so in the form of a pull request.
It would be greatly appreciated :)

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

- boost
- nlohmann\_json
- C++20 [compiler](https://en.cppreference.com/w/cpp/compiler_support/20)
- olm
  - For end to end encryption, -DLEET\_NO\_ENCRYPTION to disable
- openssl
  - For end to end encryption, -DLEET\_NO\_ENCRYPTION to disable

To install these dependencies on **Debian**:

- `apt install meson nlohmann-json3-dev libolm-dev libssl-dev libboost-dev`
- Note that libolm is not available from standard Debian bookworm repositories.
A meson wrap is included, which can be used if necessary.

To install these dependencies on **Arch**:

- `pacman -S meson nlohmann_json libolm openssl boost`

## Compiling with meson (Microsoft Windows/macOS/Linux/BSD)

You can compile libleet using the meson build system
on Windows, macOS, Linux and other Unix-like
operating systems.

You can do it like this:

- `meson setup build --prefix=/usr --reconfigure`
- `cd build`
- `meson install` as superuser.

If a dependency is not available, meson will fetch the source
code and attempt to build it to satisfy the dependency.

## Compiling with Visual Studio (Microsoft Windows)

To compile using Visual Studio 2022, you can open the solution file
and install the dependencies using nuget. All dependencies can be
installed this way, except for libolm.

To install libolm, you need to compile the CMake project manually.
You can do this by simply cloning [olm](https://gitlab.matrix.org/matrix-org/olm)
using Git. I recommend building a Release rather than a Debug build,
and I recommend that you statically link.

Then you can link with the resulting header files and the library
and then build libleet using msvc. You should end up with either
a dynamic library (.dll) or a static library (.lib)

## Use in projects

To use libleet in projects:

```cpp
/* Include libleet
 * On Linux, the binary can be found in /usr/lib and the headers
 * can be found in /usr/include/libleet.
 * If you're using BSD or Windows, this may differ. You can use
 * pkg-config to find the appropriate path and linker flag.
 */
#include <libleet/libleet.hpp>
```

If you need to use the URL parser externally, also include
libleet/net/Request.hpp.

Due to rather large dependencies, it is not recommended that you
statically link your program that depends on libleet. It's possible,
but not ideal in my opinion.

## Generating documentation

Doxygen is used project-wide for generating documentation.

To generate documentation: `cd docs; doxygen; cd ..`

## Design goals

- Fast
- Easy to use
- Cross-platform
- Make use of C++ features where appropriate, whilst not being too intimidating
- Support Element as well as possible
- Simplify Matrix API calls so that developers can just focus on
designing their user interface

## Features to implement

See the Issue tracker, as well as 'Not yet supported'.

## Examples

You can find examples in the examples subdirectory.
There are many examples that use different features
of the library. If you plan on writing a client, you should check
out `basic-chat-client` and perhaps `rest-api`.

You can build one of the examples using meson, in the same way
libleet is built.

## License

The project is licensed under the GNU Lesser General
Public License version 3.0, which means it is
free software as defined by the Free Software Foundation.

Copyright (c) 2023-2024 speedie (https://speedie.site)
