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
