# BlockRS - Blockchain Simulation in a Localized Environment

## Overview

BlockRS is an open-source initiative that demonstrates blockchain technology principles in a localized environment. This project implements core blockchain concepts including distributed networks, mining, transaction signing, and utilizes the POSIX API.

While a Win32 frontend has been conceptualized, it is currently not available due to development constraints. A Dockerfile is provided as an alternative for cross-platform compatibility.

For a comprehensive understanding of the systems design, please refer to the [BlockRS Whitepaper](#) (link forthcoming).

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
    - [Docker Installation](#docker-installation)
    - [Manual Installation](#manual-installation)
- [Building the Project](#building-the-project)
- [Usage Guide](#usage-guide)
    - [Starting the Blockchain Server](#starting-the-blockchain-server)
    - [Running a Node](#running-a-node)
    - [Running a Wallet Server](#running-a-wallet-server)
- [API Reference](#api-reference)
    - [Blockchain Server Endpoints](#blockchain-server-endpoints)
    - [Node Server Endpoints](#node-server-endpoints)
    - [Wallet Server Endpoints](#wallet-server-endpoints)
- [Contributing](#contributing)
- [Support](#support)
- [License](#license)

## Prerequisites

- Git
- CMake (3.10 or higher)
- C++ Compiler (with C++17 support)
- Docker (optional, but recommended)
- Unix-like environment (for non-Docker installations)

## Installation

Begin by cloning the repository with its submodules:

```bash
git clone --recurse-submodules https://github.com/shrehanrajsingh/blockrs.git
cd blockrs
```

### Docker Installation

*Documentation forthcoming*

### Manual Installation

While Docker is the recommended approach for consistent environments, manual installation is possible on Unix-like systems.

> **Note**: For Windows users, please use Docker or a Unix environment shell like Cygwin.

The project depends on two libraries in the `lib/` folder:
- `bdwgc/`: A garbage collector (experimental)
- `secp256k1/`: Library for encrypted signing

The `bdwgc` library is automatically included by CMake, but `secp256k1` requires manual building with specific configuration flags.

Build the `secp256k1` library with the following commands:

```bash
cd lib/secp256k1
./autogen.sh
./configure --enable-module-recovery
make
sudo make install
```

This installation places:
- `libsecp256k1.a` in `/usr/local/lib`
- Header files in `/usr/local/include/secp256k1.h` and related locations

> **Note**: If you discover a method to integrate secp256k1 directly into the CMake configuration, please share it. This would help reduce the dependency on Docker. Docker support will be maintained regardless to facilitate running networks in dedicated servers beyond localhost.

## Building the Project

Once the dependencies are in place, build the project by executing these commands from the root directory:

```bash
mkdir build
cmake --build build --config Debug --target all
```

This build process will generate several components:
- `gc`: Garbage collector
- `cord`: String handling library
- `blockrs`: The main BlockRS library
- `TEXE`: Test executable

If you encounter any build issues, please check the error messages for missing dependencies or configuration problems. Feel free to open an issue on GitHub for assistance.

## Usage Guide

BlockRS is designed as a library (`libblockrs`) to enable integration into various projects and potentially support different programming languages through API integration.

While a dedicated standalone executable is in development, you can use the test executable to explore the functionality.

### Starting the Blockchain Server

To start the blockchain server on port 8000:

```bash
cd build/tests
./TEXE -s chain -p 8000
```

### Running a Node

In a separate terminal, start a node server on port 8100:

```bash
./TEXE -s node -p 8100
```

Connect this node to the blockchain network with:

```bash
curl -X POST http://127.0.0.1:8100/connect \
         -H "Content-Type: application/json" \
         -d '{"url": "127.0.0.1:8000"}'
```

### Running a Wallet Server

In another terminal, run a wallet server on port 9000:

```bash
./TEXE -s wallet -p 9000
```

> **Note**: Node servers have built-in wallets to receive currency rewards when mining blocks. Future versions will allow linking external wallet servers to node servers.

Once all components are running, you can access the web interfaces:
- [Blockchain Server](http://127.0.0.1:8000)
- [Node Server](http://127.0.0.1:8100)
- [Wallet Server](http://127.0.0.1:9000)

If you need guidance at any step, please refer to the video tutorials in the `docs/video/` folder or reach out via email.

## API Reference

### Blockchain Server Endpoints

| Endpoint | Methods | Description |
|----------|---------|-------------|
| `/` | GET, POST, HEAD | Basic server information |
| `/info` | GET | View the entire blockchain as JSON |
| `/nodes` | GET, HEAD | View all connected nodes |
| `/transaction/new` | GET, POST | Add a new transaction (requires a Wallet Server) |
| `/transaction/all` | GET, POST | View all pending and rejected transactions |
| `/addnode` | POST | Add a node to the network (used internally) |

Example of adding a node:
```bash
curl -X POST http://127.0.0.1:8000/addnode \
        -H "Content-Type: application/json" \
        -d '{"url": "<nodeserver url>"}'
```

### Node Server Endpoints

| Endpoint | Methods | Description |
|----------|---------|-------------|
| `/` | GET, POST | Basic server information |
| `/info` | GET | View server properties as JSON |
| `/connect` | POST | Connect to a blockchain server |
| `/mine` | GET | Mine a block (one thread at a time) |
| `/update` | GET | Fetch latest block data from blockchain |
| `/wallet` | GET | View wallet information |
| `/wallet/sign` | POST | Sign a transaction using node's wallet |
| `/wallet/verify` | POST | Verify a message signature |

Example of connecting to a blockchain server:
```bash
curl -X POST http://127.0.0.1:8100/connect \
        -H "Content-Type: application/json" \
        -d '{"url": "<blockchain server url>"}'
```

Example of signing a message:
```bash
curl -X POST http://127.0.0.1:8100/wallet/sign \
        -H "Content-Type: application/json" \
        -d '{"nonce": ..., "to": "...", "value": ..., "gas_fee": ..., "data": ...}'
```

Example of verifying a message:
```bash
curl -X POST http://127.0.0.1:8100/wallet/verify \
        -H "Content-Type: application/json" \
        -d '{"message": ..., "sign": "<signature>"}'
```

### Wallet Server Endpoints

| Endpoint | Methods | Description |
|----------|---------|-------------|
| `/` | GET, POST | Basic server information |
| `/info` | GET | Get wallet information (public key and address) |
| `/sign` | POST | Sign a message |
| `/verify` | POST | Verify a message against a signature |

Example of signing a message:
```bash
curl -X POST http://127.0.0.1:9000/sign \
        -H "Content-Type: application/json" \
        -d '{"message": ...}'
```

Example of verifying a message:
```bash
curl -X POST http://127.0.0.1:9000/verify \
        -H "Content-Type: application/json" \
        -d '{"message": ..., "signature": "<signature>"}'
```

If you're unsure about how to use these endpoints, please check the video tutorials in the `docs/video/` directory for detailed guidance.

## Contributing

Contributions to BlockRS are welcome! Please see the [CONTRIBUTING](./CONTRIBUTING) file for guidelines.

## Support

If you need assistance or have questions, please contact:

- Email: shrehanofficial@gmail.com or shrehanrajsingh.24@kgpian.iitkgp.ac.in
- GitHub: [shrehanrajsingh](https://github.com/shrehanrajsingh)
- LinkedIn: [Shrehan Raj Singh](https://www.linkedin.com/in/shrehan-raj-singh-28a1a0242/)

## License

This project is licensed under the terms of the [LICENSE](./LICENSE) file.

---

Developed by [Shrehan Raj Singh](https://github.com/shrehanrajsingh)