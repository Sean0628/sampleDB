# sampleDB
## Usage (Run in Docker)
### 1. Build and enter the container
```sh
$ docker-compose run --rm app bash
```

### 2. Build and run all tests
```sh
$ make run
```
This compiles all source and test files into `build/test_runner` and executes them.

### 3. Clean up build artifacts
```sh
$ make clean
```

## Implementation
### File Manager
#### `src/file/BlockId.cpp`
This class represents a logical identifier for a block in the database file system.
A `BlockId` uniquely identifies a block by its file name and block number.

#### `src/file/Page.cpp`
This class represents a page (in-memory buffer) in the database file system.

It stores data in an in-memory byte buffer whose size is typically equal to the block size.
Using the methods provided by this class, users can read and write typed data (e.g., integers and strings) at specific offsets within the page.

The `Page` class does not perform disk I/O and is independent of file storage.

#### `src/file/FileMgr.cpp`
This class manages the low-level file and disk I/O operations for the database.

It is responsible for reading and writing entire blocks between disk and memory by transferring data between `BlockId`s and `Page`s.
`FileMgr` also handles file creation, block allocation, and concurrency control for file access.
