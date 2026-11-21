# A simple web server for learning

## Prerequisites

- GCC and Make installed on a Linux system

## How to compile

- Clone the repo and run a make command (make, make run, make release)
- The binary will build to src/app (for a debug build or src/app_release for a release build)
- The server listens 0.0.0.0:8080 by default, but can be changed from header/config.h

## Features

- Responds to HTTP-requests
- Custom URL paths can be mapped to HTML files

## Limitations

- Single threaded
- Supports only get requests and gives 405 on other request types
- Not safe by any stretch of the imagination

## How to add custom URL paths

- Edit the src/config.c file's if/else chain like in the examples
- Recompile the program
