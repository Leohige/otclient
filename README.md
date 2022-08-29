# OTCLient - Ancient Redemption
[![MIT License](https://img.shields.io/apm/l/atomic-design-ui.svg?)](https://github.com/tterb/atomic-design-ui/blob/master/LICENSEs)
[![Build - Windows](https://github.com/Leohige/otclient/actions/workflows/build-windows.yml/badge.svg?branch=main)](https://github.com/Leohige/otclient/actions/workflows/build-windows.yml)
[![Build - Linux](https://github.com/Leohige/otclient/actions/workflows/build-ubuntu.yml/badge.svg?branch=main)](https://github.com/Leohige/otclient/actions/workflows/build-ubuntu.yml)
[![Build - MacOS](https://github.com/Leohige/otclient/actions/workflows/build-macos.yml/badge.svg?branch=main)](https://github.com/Leohige/otclient/actions/workflows/build-macos.yml)

### Based on [mehah/otclient](https://github.com/mehah/otclient)

### Features

- No Tibia 12.85 Support
- No protobuf dependency
- Conan Package Manager
- All Mehah OTClient Improvements

### Active Developers
[@leohige](https://github.com/Leohige)

<h2>
  
```diff
- Want to help? Just open a PR.
```
  
</h2>

### What is otclient?

Otclient is an alternative Tibia client for usage with otserv. It aims to be complete and flexible,
for that it uses LUA scripting for all game interface functionality and configurations files with a syntax
similar to CSS for the client interface design. Otclient works with a modular system, this means
that each functionality is a separated module, giving the possibility to users modify and customize
anything easily. Users can also create new mods and extend game interface for their own purposes.
Otclient is written in C++20 and heavily scripted in lua.

For a server to connect to, you can build your own with the [forgottenserver](https://github.com/otland/forgottenserver)
or connect to one listed on [otservlist](https://otservlist.org/).

### Where do I download?

Binaries for Windows and Linux can be found here (but can be outdated):

- [Release Builds](https://github.com/leohige/otclient/releases)

**NOTE:** You will need to download spr/dat files on your own and place them in `data/things/VERSION/` (i.e: `data/things/1098/Tibia.spr`)

### Compiling

In short, if you need to compile OTClient, follow these tutorials:
* [Compiling on Windows](https://github.com/leohige/otclient/wiki/Compiling-on-Windows)
* [Compiling on Linux](https://github.com/leohige/otclient/wiki/Compiling-on-Linux)
* [Compiling on OS X](https://github.com/leohige/otclient/wiki/Compiling-on-Mac-OS-X)

### Need help?

Try to ask questions in [otland](http://otland.net/f494/), now we have a board for the project there,
or talk with us at the gitter chat.

### Bugs

Have found a bug? Please create an issue in our [bug tracker](https://github.com/leohige/otclient/issues)

### Contributing

We encourage you to contribute to otclient! You can make pull requests of any improvement in our github page, alternatively, see [Contributing Wiki Page](https://github.com/leohige/otclient/wiki/Contributing).

### Contact

Talk directly with me at Discord [Leohige#9237](https://discord.com/users/580153378944974849), or send me an email: [leohige@outlook.com](mailto:leohige@outlook.com)

### License

Otclient is made available under the MIT License, thus this means that you are free
to do whatever you want, commercial, non-commercial, closed or open.
