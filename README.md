# steampunk-game

A victorian-steampunk-themed game developed primarily in TypeScript -- a superset of
JavaScript -- using a custom game engine built to be compatible with many standard
browsers. This project is also designed to run natively using [Tauri](https://tauri.app/).

This project is a work in progress and its engine, APIs, resources, and other features
are subject to change at any time.

## Prerequisites

### Git
[Git](https://git-scm.com/) is required to clone and make changes to this project. It can
be downloaded from the Git website or your system's package manager, such as `apt`,
`pacman`, or `winget`.

If you plan to make changes to this project, it's recommended (but not required) to use a
GUI client for Git. Some clients include Visual Studio Code's source control tab,
GitKraken, or GitHub Desktop.

### JavaScript Runtime
A JavaScript runtime is used for TypeScript compilation, Webpack bundling, and Tauri
packaging. Here are some recommended runtimes:

- [Node.js](https://nodejs.org/en/) is recommended for Windows or Unix-like platforms
- [Bun](https://bun.sh/) is preferred for Linux and other Unix-like platforms because
    it is a faster runtime.

You do not need to install a JavaScript runtime if you are a client playing
the game from a hosted server.

### Node Package Manager
You will also need a package manager that is either compatible with NPM or NPM itself.
NPM is packaged with Node.JS in most cases, but you can also install it as a standalone
through your package manager or an install script.

Bun has a built-in package manager that is compatible with NPM, and it is recommended to
be used if you are using Bun. A [Standalone](https://www.npmjs.com/package/npm) is also
available for runtimes that do not include NPM.

### Tauri Prerequisites
If you are planning to build the project under Tauri, you will need to install specific
[Prerequisites](https://tauri.app/v1/guides/getting-started/prerequisites) depending on
your target platform.

You do not need to worry about installing Tauri's command line application. The Tauri CLI
will locally install with the project.

## Preparation

To prepare the project, you will need to clone the repository and install the project's
dependencies. You can clone the repository using the following command:
```shell
git clone <repository-location>
```

You can install the project's dependencies using the following command:
```shell
npm install # or bun install
```

You may need additioal dependencies depending on your target platform. See the
[Tauri Prerequisites](#tauri-prerequisites) section for more information.

Bun is a faster alternative to Node.JS that provides backwards compatibility with NPM and
Node.JS's API and is preferred for Linux and other Unix-like platforms. Windows support
for Bun is currently experimental and not recommended.

<center>
    <img alt="Works on My Machine" width="64" src="https://i.imgur.com/9du7dMh.png" />
</center>

This project runs fine on Windows Subsystem For Linux v2.0 (WSL2) + WSLg (WSL GUI), but
the game's performance is significantly reduced and there is no guarantee that running the
game on WSL will work on all systems.

## Running

To run the game in a browser, you will need to run a server to host the game's files. This
project uses [Webpack](https://webpack.js.org/) to bundle the game's files into a single
file. The project has a script to run Webpack's development server.
```shell
npm run start # or bun run start
```

You can open the game in your browser by navigating to [http://localhost:3000](http://localhost:3000).

## Building

You can compile the game into a native application using [Tauri](https://tauri.app/). The
Tauri CLI is included with the project and can be used to build the game for your target
platform.

To compile the game for your target platform, you can use the following command:
```shell
npm run tauri build # or bun run tauri build
```

To run the game in development mode with Webpack's development server, you can use the
following command:
```shell
npm run tauri dev # or bun run tauri dev
```

The former command will compile the game into a native application for your target
platform, and the latter command will run unoptimized Tauri with the Webpack development
server.

You do not need to run a separate server instance when running `tauri dev` because Tauri
will automatically run the Webpack development server.
