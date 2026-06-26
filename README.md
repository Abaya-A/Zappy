<div id="top">

<div align="center">

# <code> ZAPPY </code>


tools and technologies used:

![Static Badge](https://img.shields.io/badge/CMake-064F8C.svg?style=default&logo=CMake&logoColor=white)
![Static Badge](https://img.shields.io/badge/cpp-blue?style=flat&logo=c%2B%2B&logoColor=white)
![Static Badge](https://img.shields.io/badge/python-yellow?style=flat&logo=python&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/pytorch-red?style=flat&logo=pytorch&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/gymnasium-black?style=flat&logo=react&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/yaml-blue?style=flat&logo=yaml&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/matplolib-green?style=flat&logo=matplotlib&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/imageio-orange?style=flat&logo=imageio&logoColor=white&logoSize=auto)
![Static Badge](https://img.shields.io/badge/numpy-white?style=flat&logo=numpy&logoColor=black&logoSize=auto)
![Static Badge](https://img.shields.io/badge/swig-grey?style=flat&logo=swig&logoColor=black&logoSize=auto)

</div>

---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Acknowlegments](#acknowlegments)

---

## Overview

The goal of this project is to create a network game where several teams confront each other on a tile map
containing resources.
The winning team is the ﬁrst one where at least 6 players reach the maximum elevation.

---

## Features

❯ field to fill

---

## Project Structure

```sh
└── /
    ├── ai
    │   └── foraifile
    ├── docs
    │   └── devdoc.md
    ├── gui
    │   └── forguifile
    ├── server
    │   └── forerverfile
    ├── tests
    │   └── fortestfile
    ├── CONTRIBUTING.md
    └── Makefile
```

---

## Getting Started

### Prerequisites

This project requires the following dependencies:

- **Programming Language:** CPP, Python, Rust
- **Package Manager:** Makefile
- **Shared Libraries:** Pytorch, Gymnasium, Yaml, matplotlib, imageio, numpy, swig

### Installation

Build from the source and install dependencies:

1. **Clone the repository:**

```sh
git clone git@githubcom:EpitechPGE2-2025G-YEP-400-STG-4-1-zappy-3.git
```

2. **Navigate to the project directory:**

```sh
cd
```

3. **Install the dependencies:**

Dependencies are installed with the AI Makefile.

### Usage

Run the project with:

**Using [Makefile](https://gl.developpez.com/tutoriel/outil/makefile/):**

#### Server

```sh
$> cd server
$> make
$> ./zappy_server --help
USAGE: ./zappy_server -p port -x width -y height -n name1 name2 ... -c clientsNb -f freq
```

#### Gui

```sh
$> cd gui
$> make
$> ./zappy_gui --help
USAGE: ./zappy_gui -p port -h machine
```

#### AI

```sh
$> cd ai
$> make
$> ./zappy_ai --help
USAGE: ./zappy_ai -p port -n name -h machine
```

### Testing

uses the text folder, run the test suite with:

**Using [Makefile](https://gl.developpez.com/tutoriel/outil/makefile/):**

```sh
make test
```

---

## Roadmap

- [X] **`Task 1`**: ~~Project management.~~
- [] **`Task 2`**: CI/CD workflow with a mirror.
- [X] **`Task 3`**: build Zappy Ai.
- [X] **`Task 4`**: build Zappy server.
- [X] **`Task 5`**: build Zappy gui.
- [] **`Task 6`**: update user and dev doc.

---

## Contributing

- **💬 [Join the Discussions](https://LOCAL///discussions)**: Share your insights, provide feedback, or ask questions.
- **🐛 [Report Issues](https://LOCAL///issues)**: Submit bugs found or log feature requests for the `` project.
- **💡 [Submit Pull Requests](https://LOCAL///blob/main/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your LOCAL account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone ./
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to LOCAL**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!
</details>

<details closed>

## License

 is protected under the [LICENSE](https://choosealicense.com/licenses) License. For more details, refer to the [LICENSE](https://choosealicense.com/licenses/) file.

---

## Acknowlegments

- Credit `contributors: 8kao, RvbyY, younes-cod, Snogyy, leolcde`

[![][back-to-top]](#top)
</div>

[back-to-top]: https://img.shields.io/badge/-BACK_TO_TOP-151515?style=flat-square

---
