# HW3 Short Answer Questions  
With each HW Assignment, there will be a few short answer questions for you to fill out.  
These questions will hopefully require you to reflect back on your experiences with the assignment.  
Please write at least a few sentences for each question.

## Question 1
##### HW3 had you writing the in-memory structure from HW2 into a file on disk. What are some (>=2) advantages of storing this structure to a file? What is a disadvantage?
An advantage is that we are able to process our data/run the program very
quickly if we write to a file on disk because of the cache.  Another advantage
is that data can be read across many platforms simply by converting to host
or network byte order.  A disadvantage though were if this indexing was important
and the file got corrupted, then it is possible that it may be non-recoverable.

## Question 2
##### At this point you have written some very similar non-trivial programs in C and C++. In HW2 you implemented searchshell.c and Memindex.c, while in this HW 3 you implemented the similar programs QueryProcessor.cc and filesearchshell.cc. List (>=2) notable differences in the experience you had writing in C vs in C++.
I've attempted writing (didn't finish yet, we'll see) the QueryProcessor for 
this as it similar to Memindex but slightly more complex, so I'll respond to the
insight on that.  A notable difference in searching is that we have strings to 
use and a very handy string.compare() unlike C.  Another notable difference is

## Question 3
##### Note that in FileIndexReader.cc STEP 1, we set it so that reading/writing to the index file was unbuffered. We do this so that we can use our own strategy of reading the "in-file" HashTable, and avoid the default buffering strategy provided by the C standard library. Why would turning off the buffer be more efficient than leaving it on in our use case of reading files?  
###### (Recall that if you were to read the first byte of a file with fread and buffering on, it would instead read the first many (likely >= 512) bytes and store it in an internal buffer before returning the one byte you requested. This is so that if you want to read the next byte in the file, fread() doesn't have to go to the OS and disk to read it, instead just returning the bytes it already read).
Turning off the buffer is more efficient for the cases we are considering:
reading/writing operations for pieces of memory and jumping around.  We are 
utilizing offsets to write very specific bits such as headers, sizes, metadata,
etc. and would not require to write/read only when the buffer is full.  Without
the buffer on, we can perform these operations as soon as possible and this is
useful for us as we don't need the many bytes and just a specific metadata info,
which we know the format of, size, and all the nice details.  :)