Limitation: Not sure how would the things looks like if the path name ends with "/"

TODO:
    - Following info are not updated in "Inode" meta data (both dir and file)
        - sizes (size of the cur obj)
        - links (total no of reference that points to this particular file)

TO CHECK:
    - Currently our dir creation works like:
        - Create a default inode for dir, in there assign a free data region to inode
        - Come again find the same free data region again and then update on that
        - But during 1 step we point direct pointer to the free data region
        - And during 2 step we also point another/next direct pointer to the same free data region
        - after that we just checks the next direct pointer and then do the connection (as per required)
        - this dual connection should probably lead to some weird dual pointing from same dir

