# Simple Shell Project

This is a simple Unix shell implementation created as part of a project. The shell supports basic command execution, built-ins, and various features outlined in the project requirements.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project is a simple Unix shell, inspired by the original Unix shells like the Thompson shell. It is developed as part of a programming assignment to understand system calls, processes, and basic shell functionalities.

## Features

- Basic command execution
- Handling command lines with arguments
- PATH resolution for command execution
- Built-in commands: `exit` and `env`
- Logical operators: `&&` and `||`
- Variables replacement: `$?`, `$$`, `$PATH`
- Error handling and graceful termination

## Getting Started

### Prerequisites

- Ubuntu 20.04 LTS
- GCC compiler

### Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o simple_shell
