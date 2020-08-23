# HW3 Short Answer Questions  
With each HW Assignment, there will be a few short answer questions for you to fill out.  
These questions will hopefully require you to reflect back on your experiences with the assignment.  
Please write at least a few sentences for each question.

## Question 1
##### This homework had you write a program that worked very closely with the network. List a few (>=2) things that you had to account for when writing code that directly interacted with the network. (Interactions could include reading/writing to the network, managing connections, and setting up the network server in ServerSocket.cc)
One thing that I had to account for was how the network communicated and how picky
each formatting was (we had to write a method for escaping and manually change 
Content-types).  There had to be a certain way to communicate in that sense.

Another thing I noticed was there's a very procedural way networks pass word on to
each other through requests and response.  But, the common point is that they
both need the messenger: the server to do so.  So I attempted the #5 in part B
which is the hardest but it doesn't work because I'm not communicating responses
correctly so the page doesn't even load and it says that BikePaloooozzzzaaaa.html 
can't be found either.  Sorry.. ranting.  :)  Got to let it out.


## Question 2
##### In this homework, you wrote a program that worked with concurrency. List a few (>=2) notable things that you encountered when writing code or read provided code that took advantage of concurrency or was run concurrently.
One of the things I took advantage of (or rather told from the step hints) are
using while loops to keep running and waiting for a connection.  Since we are always
"locking" or "blocking" until we receive a connection.  We took advantage of this
aspect of concurrency.

Another point is we run our server tasks concurrently (provided code to Run in
HttpServer) through the threadpool.
This is essential to any iteractive program as this means at any point something
could be running but it may be one of the last things running before a user cancels
or stops the connection.  Once that happens, the program should stop the task
receiving too.



## Question 3
##### Why do we try to read many bytes at a time with WrappedRead when we call HttpConnection::GetNextRequest? Please explain your reasoning. 
Reiterating a point I mentioned in question 2, reading many bytes at a time allows
us to spot the stopping point of work sooner (or exit if the header is malformed.)
As we would have to process what is read anyways at some point, we do know that 
it would not make sense to read something one byte at a time if it ends up
being a malformed header for example.. so we can reach the point of
processing faster if we take in the bytes in larger chunks.  It's also inefficient
to call one byte at a time because our requests have args that are greater than
a byte as well.  We don't split byte by byte!  TLDR; we don't need to read byte by
byte since we'll revisit and need to append what comes after the header end
for the next time's request buffer so we should add as much possible.


