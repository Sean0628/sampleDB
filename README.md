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

### Log Manager
#### `src/logging/LogMgr.cpp`
This class manages the database write-ahead log (WAL) file at a low level. It appends log records, assigns increasing LSNs, packs records into fixed-size log blocks, and flushes log data to disk when requested to support durability and crash recovery. Transaction atomicity and WAL write-ordering guarantees are enforced by higher-level components that coordinate when log records are flushed and when data pages may be written.

### Buffer Manager
#### `src/buffer/Buffer.cpp`
This class represents a buffer frame in the buffer pool of the database system. It holds a `Page` in memory and tracks metadata such as the associated `BlockId`, pin count, and dirty status. A `Buffer` is responsible for flushing its contents to disk when required, while enforcing write-ahead logging (WAL) by ensuring the corresponding log records are flushed before the page is written.

#### `src/buffer/BufferMgr.cpp`
This class manages the buffer pool, which is a collection of fixed-size `Buffer` frames that cache database pages in memory. It handles pinning and unpinning pages, selecting a buffer frame to reuse when the pool is full, and coordinating the flushing of dirty pages to disk.

For simplicity, this implementation uses a Naive replacement strategy, which selects the first unpinned buffer it finds for replacement.
