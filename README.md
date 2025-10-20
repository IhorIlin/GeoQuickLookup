# Geolocation lookup
Efficient IP-to-Location lookup implementation in C++ with a preprocessed binary database format and memory-mapped data access.

## Overview
This project loads a large IP-to-location database (`database.csv`) and performs high-speed lookups for given IPv4 addresses.

Two main components are included:

1. **Geo_processor** - converts a CSV file into a compact binary database (`database.bin`).
2. **Sample_app** - loads the binary database using **memory mapping (mmap)** and performs lookups efficiently in O(log N) time.

## Supported OS
Solution was designed for `Linux` and `MacOS` operating systems.

## Design

### On-disk data format (`database.bin`)

The binary file is composed of:
```
Header
Entries []
StringTable 
```
Header struct description:
```
struct Header {
    char magic[4];      Signature "NORD"
    uint32_t count;     Number of entries
    uint32_t str_bytes; Total size of the string table (for validation)
};
```
Entry struct description:
```
struct Entry {
    uint32_t start;        First IP address in range
    uint32_t end;          Last IP address in range
    uint32_t label_offset; Offset from file start ex. "CC, City" string
};
```
## In-memory layout

When the app starts, the binary file is **memory-mapped** using `mmap(2)`:
```
mmap region -> Header -> Entries -> Strings
```
No parsing or copying occurs the file is accessed **directly from memory**, giving near-zero load time and very low memory overhead.

Lookups are implemented as a **binary search** over sorted IP ranges (`Entry.start`).

---

## Performance (MacBook Pro M1, 16 GB RAM)

| Metric              | Value       |
|---------------------|-------------|
| Load time           | ~0.1 ms     |
| Memory usage        | ~1.3 MB     |
| Average lookup time | 20–40 µs    |
| Score (test script) | ~50+ points |

## Performance (Ubuntu 24.04.2 LTS (GNU/Linux 6.8.0-83-generic x86_64) 4 GB RAM)

| Metric              | Value        |
|---------------------|--------------|
| Load time           | ~231 µs      |
| Memory usage        | ~4.3 MB      |
| Average lookup time | 30–125 µs    |
| Score (test script) | ~120+ points |
---
## Preprocessing

The preprocessing converts CSV data into a compact binary format.  
Implemented in `preprocess.cpp`.

### Algorithm
1. Parse each CSV line (`start`, `end`, `countryCode`, `cityName`).
2. Sort rows by `start` IP.
3. Write:
  - Header (`NORD`, `count`, `str_bytes`)
  - All entries (`Entry` structs)
  - All `"CC,City\0"` strings.

### Lookup logic
Implemented in database.cpp and database.h:
1.	Open database.bin using mmap.
2.	Validate header ("NORD", file size consistency).
3.	Perform binary search:
```
size_t left = 0;
size_t right = header_->count;
while (left < right) {
    size_t middle = (left + right) / 2;
    const Entry &entry = entries_[middle];

    if (ip < entry.start) {
        right = middle;
    } else if (ip > entry.end) {
        left = middle + 1;
    } else {
        return stringTable_ + entry.label_offset;
    }
}
```
4. Return "CC,City" or nullptr.
---

## Manual compile and setup 

In order to build and test the project manually, make sure the following tools are installed on your system:

| Tool                   | Linux                         | macOS                           |
|------------------------|-------------------------------|---------------------------------|
| CMake                  | sudo apt install cmake        | brew install cmake              |
| Make                   | sudo apt install make         | (included with Xcode CLI tools) |
| g++ / clang++          | sudo apt install g++          | (included with Xcode CLI tools) |
| Python 3               | sudo apt install python3      | brew install python3            |
| pip                    | sudo apt install python3-pip  | (included with Homebrew Python) |
| psutil (Python module) | python3 -m pip install psutil | python3 -m pip install psutil   |

## Optional utilities
**gunzip** - used to decompress `database.csv.gz`
```
sudo apt install gzip - for Linux
```
```
brew install gzip - for macOS
```
You can verify everything is installed correctly by running:
```
cmake --version
g++ --version
python3 --version
```
From root folder of repo perform these commands:
```bash

# 1. Create build directory
mkdir -p geolocation/build
cd geolocation build

# 2. Generate Makefiles
cmake ..
 
# 3. Compile all targets
make 
```
## Run Preprocessing and Tests
```bash

# Make sure that you unzip database.csv.gz file performing command
gunzip -k database.csv.gz (-k argument to keep original file)

# Generate binary database file
./Geo_processor ../database.csv ../database.bin

# Run validation tests
python3 ../geolocation_test.py --executable ./Sample_app --database ../database.bin
```

##  Automatically build and test the project
Open your terminal in the project’s root folder (where the README.md is) and run:
```bash

bash setup_and_run.sh 
```
In console output you should see test results - for example:
```
Database loaded Memory usage: 3.38mb Load time: 182μs
OK    1.0.0.0 US Los Angeles Memory usage: 4.0mb Lookup time: 182μs
OK    71.6.28.0 US San Jose Memory usage: 4.38mb Lookup time: 81μs
OK    71.6.28.255 US San Jose Memory usage: 4.38mb Lookup time: 30μs
OK    71.6.29.0 US Concord Memory usage: 4.38mb Lookup time: 26μs
OK    53.103.144.0 DE Stuttgart Memory usage: 4.75mb Lookup time: 77μs
OK    53.255.255.255 DE Stuttgart Memory usage: 4.75mb Lookup time: 26μs
OK    54.0.0.0 US Rahway Memory usage: 4.75mb Lookup time: 21μs
OK    223.255.255.255 AU Brisbane Memory usage: 5.25mb Lookup time: 94μs
OK    5.44.16.0 GB Hastings Memory usage: 5.5mb Lookup time: 54μs
OK    8.24.99.0 US Hastings Memory usage: 5.75mb Lookup time: 53μs
Final points for 10 measurements:  122.686581
```

---










# A challenge for Great Low Level Developer

## Prerequisites
You should have received these files:
- README.md
- geolocation/database.csv.gz
- geolocation/sample_app.cpp
- geolocation/geolocation_test.py
- geolocation/CMakeLists.txt

Note: `geolocation/database.csv.gz` will have to be unziped to get raw database.csv file

## Specification
Using systems programming language of your choice (C/C++/rust/go/zig...) build
a command line application that performs geolocation lookups for given IP addresses.

To do the lookup, use the supplied geolocation information from .csv file, (later
on referred to as "database" file).

### The Protocol
Your application is expected to implement specific line-based request-response
protocol for communication with other processes.

The protocol itself consists only of three commands:
* LOAD
* LOOKUP <IPv4 address>
* EXIT

#### LOAD
LOAD command is used for loading the database into memory. The application
should respond with the string `OK` once the database is fully loaded. Note, as
this is line-based protocol, the string `OK` must end with new line.

Example command execution:
```
> LOAD
< OK

```

Here `>` means data that your app receives via stdin, and `<` means data your
app writes into the stdout.

For error cases, "ERR" should be printed as a response, for example:
```
> LOAD
< ERR

```
No need to go deeply into the error codes/messages etc. just an "ERR" string is enough.

#### LOOKUP
LOOKUP command is used for performing geolocation lookup. The application
should respond with the location formated as `<COUNTRY CODE>,<CITY>`, there
should be no spaces before or after the comma. The response must end with
new line.

Example command execution:
```
> LOOKUP 71.8.28.3
< US,Hammond

```

Here `>` means data that your app receives via stdin, and `<` means data your
app writes into the stdout.

For error cases, "ERR" should be printed as response, for example:
```
> LOOKUP 1.2.3.4.5
< ERR
```
No need to go deeply into the error codes/messages etc. Just an "ERR" string is enough.


#### EXIT
EXIT command indicates that the application should now exit. Note that before
exiting, the application should respond with OK. The response must end with
new line.

Example command execution:
```
> EXIT
< OK

```

Here `>` means data that your app receives via stdin, and `<` means data your
app writes into the stdout.

---

You can use the provided `geolocation/geolocation_test.py` script to test the
compatability of your implementation.

### The Goal
Your main goal is to make this geolocation app as efficient as possible. Any
and all optimizations are on the table as long as CLI protocol is adhered and it
can help lower the resource usage.

More specifically by "resource usage" we mean
* load time
* lookup time
* memory usage
in these proportions:

```
points = load_time_ms + memory_usage_mb * 10 + lookup_time_ms * 1000
```

The goal is to collect as low amount of points as possible.

In order to achieve the best performance, you are encouraged to preprocess the
database file into your own, custom, on-disk format, which will be used during
database loading.

The same goes for in-memory data structures. You are encouraged to research and
implement any structures/algorithms which will help you lower the points.

While Your solution should handle other datasets gracefully, optimizations can and
should be tailored to this specific dataset. In case the optimization is not compatible
with other datasets one can just return an error during preprocessing or loading.

In general any optimization is allowed as long as original geolocation_test.py
script (or the CLI protocol) is not modified. Even ones which feels like cheating.

Therefore choosing a good approach for this task is the most important choice, so
do not be afraid to use your creativity and _destroy_ the numbers!

Note: measurements will be performed on hot page-cache.

### Requirements
- Provide README, explaining how to compile and setup the program
- Specify operating system it is designed for
- Document your approach at solving the problem, especially in-memory
  and on-disk data structures
- If you choose to use preprocessed database format, provide a script or some
  program (with source code) which was used to do preprocessing.
- Provide expected performance figures (in case we would get lower speeds
  due to some minor mistake, we will contact you, to resolve the issue)
- You can use any 3rd party libraries/frameworks (although you may need to
  explain why it's needed, and how it works internally)

---

### Score Board

```
+----------------------+----------------+---------------+
|         Name         |       OS       | Average score |
+----------------------+----------------+---------------+
| The biggest chad     | Windows        |            61 |
| Sage                 | Linux          |            89 |
| Optimizatron         | Linux          |           200 |
| Chionger             | Linux          |           301 |
| Leader of the memory | Linux          |           811 |
| Points Burner        | Linux          |          1061 |
| Combo Breaker        | Linux          |          1760 |
| Trix                 | MacOS          |          1824 |
| Hackeromatic         | Linux          |          1930 |
| Poseidon             | Linux          |          1943 |
| Tracer               | Linux          |          2106 |
| Bytes Maestro        | Linux          |          4132 |
| Leaky boi            | Cross-platform |          5700 |
| James May            | Cross-platform |         51132 |
+----------------------+----------------+---------------+
```

Disclaimer:
All the scores are measured on rather average laptop.
If you are comparing those scores - take them with a grain of salt, as different
computers may perform a little bit differently.

Good luck!
