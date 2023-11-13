# steampunk-game

A victorian-steampunk-themed game developed in TypeScript using a custom game engine built
to be compatible with many standard browsers.

This project is also designed to run natively using [Tauri](https://tauri.app/).

## Prerequisites

- Git Interface
    - [Git](https://git-scm.com/) is required to clone and make changes to this project.
- JavaScript runtime
    - The JavaScript runtime is used for TypeScript compilation, Webpack bundling, and
    Tauri packaging.
    - [Node.js](https://nodejs.org/en/) is recommended for Windows or Unix-like platforms
    - [Bun](https://bun.sh/) is preferred for Linux and other Unix-like platforms because
    it is a faster runtime.
- Node Package Manager (NPM) or any NPM-compatible package manager
    - NPM is packaged along with Node.js
    - Bun has a custom package manager that is compatible with NPM
    - A [Standalone](https://www.npmjs.com/package/npm) is also available for runtimes
    that do not include NPM.
- Tauri Prerequisites
    - If you are planning to build the project under Tauri, you will need to install
    specific [Prerequisites](https://tauri.app/v1/guides/getting-started/prerequisites)
    depending on your target platform.
    - The Tauri CLI will install with the project.

## Running

### Browser

To run the game in a browser, you will need to run a server to host the game's files. This
project uses [Webpack](https://webpack.js.org/) to bundle the game's files into a single
file. The project has a script to run Webpack's development server.
```shell
npm run start # or bun run start
```

### Native (Tauri)

You can compile the game into a native application using [Tauri](https://tauri.app/). The
Tauri CLI is included with the project and can be used to build the game for your target
platform.

To run the game in development mode with Webpack's development server, you can use the
following command:
```shell
npm run tauri dev # or bun run tauri dev
```

To compile the game for your target platform, you can use the following command:
```shell
npm run tauri build # or bun run tauri build
```