# fs-prototype: Custom File System Implementation

This project is a **custom file system prototype** designed exclusively for **educational purposes** to illustrate fundamental file system and I/O concepts in C.

**WARNING: This is a toy implementation and is not intended for use in any production-level or serious application.**

-----

## Getting Started

This section assumes you have the necessary source files and a `Makefile` configured for compilation.

1.  **Clone the Repository:**

    ```bash
    git clone [YOUR_REPOSITORY_URL]
    cd fs-prototype
    ```

2.  **Compile the Project:**
    The provided `Makefile` should be configured to compile the main demonstration program. Requires `gcc`.

    ```bash
    make
    ```

3.  **Run the Demo:**
    Execute the compiled demonstration program.

    ```bash
    ./fs_demo
    ```

4.  **Key Functions (API):**
    Once set up, the core functions for interacting with the file system typically involve:

      * **Directory Creation:** `void MakeDirectory(const char *pathPtr);`
      * **List Directory:** `void ListDirectory(const char *pathPtr);`
      * **File Creation:** `void CreateFile(const char *pathPtr);`
      * **Write To File:** `void WriteToFile(const char *pathPtr, const char *fileData);`
      * **Read File:** `void ReadFile(const char *pathPtr);`

-----

## Known Limitations

As a non-production prototype, this implementation has significant functional limitations:

  * **Path Requirements:**
      * The path must always **start with the root** (i.e., `/`).
      * Path names are limited to **smaller than 255 characters**.
  * **File/Block Size:**
      * Current **file size is assumed to be 4KB**.
      * Directory size (for storing children details) is currently **assumed to be under 4KB** (needs verification).
  * **Missing Features:**
      * No logic is implemented for **indirect references** (limiting total file size).
      * **No logic implemented for fragmentation correction or coalescing adjacent free blocks.** (Implied by a basic system, if relevant).

-----

## Development Roadmap (TODO)

The following features are prioritized for future development:

### P0 (Critical - Immediate Focus)

  * Implement the feature to **remove/delete directories and files**.
  * Implement support for **file sizes greater than 4KB** (likely by implementing indirect references).
  * Correctly update critical metadata in the **Inode** for both directories and files:
      * `sizes` (size of the current object).
      * `links` (total number of references/hard links pointing to this file).
      * Check for other essential fields like `mTime` (modification time) and `blocks`.

### P1 (High Priority)

  * Verify and resolve the assumption: Check if **Directory details for Children could be greater than 4KB**.
  * Add the feature to **append data to a file** (instead of only force-overriding the content).
  * **Remove all compiler warnings**.
  * **Refactor the code** to be more modular and improve overall structure.

### P2 (Future Enhancements)

  * Add a **basic REPL (Read-Eval-Print Loop)** to interact with the file system.
  * Add more sophisticated **directory/file differentiators/customization**.
  * Implement real use-case logic for **Permission** and other basic metadata.
  * Add logic to **save/flush the File System state** to a real disk or persistent file. This will also require a saved-state re-initializer.

-----

## References and Resources

  * OSTEP: Operating Systems: Three Easy Pieces - Chapter on File System Implementation:
      * `http://pages.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf`
