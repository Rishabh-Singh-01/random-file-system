Limitation:
    - Path should always starts with root (i.e. "/")
    - Path name should always be smaller than 255 chars

TODO:
    - Add feature for remove dir/files (P0)
    - Following info are not updated in "Inode" meta data (both dir and file) (P0)
        - sizes (size of the cur obj)
        - links (total no of reference that points to this particular file)
        - check for other stuff like mTime, blocks etc
    - Add feature for append data to file (insted of force override) (P1)
    - Remove compiler warnings (P1)
