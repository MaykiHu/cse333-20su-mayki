# HW2 Short Answer Questions  
With each HW Assignment, there will be a few short answer questions for you to fill out.  
These questions will hopefully require you to reflect back on your experiences with the assignment.  
Please write at least a few sentences for each question.

## Question 1
##### In this assignment, you had to make substantial use of the data structures in C that you have built so far. As a user of these data-structures, what difficulties did you encounter? If you did not have many difficulties, describe anything notable you encountered while using them. Write >= 3 detailed sentences.
I guess one of the difficulties was understanding how to free their payloads or
writing a payload to free them.  I would often have to refer back to the public
.h files to understand how to use the data structure -- that wasn't much a
difficulty but a thankfulness to how handy and descriptive the public functions
were.  I guess one other difficult thing was how abstract the types got
and an appreciation to how we could use the data-structures regardless of the
type as long as we had an HTKeyValue_t.


## Question 2
##### Creating the file system crawler, indexer, and search engine required working with C-strings (char*). What were a few (>=3) things you had to consider when working with char* compared to the use of a more properly defined String object that would be provided in other languages?
:( I didn't end up finishing indexer quite correctly, and didn't do search engine
but as to working with char *, I found that I had to be considerate of its length
most frequently.  Since we were dealing with char * and not some string (like 
we have in c++), we had to account for the '\0'.  We also had to think of them
as pointers, like when we were traversing it for file parsing (wordstart, curptr).
A last thing to note when working with them 


## Question 3
##### In this homework, we made use of the POSIX API to read files, and crawl directories. List a couple (>= 2) plausible reasons as to why we used POSIX instead of the C standard library functions for this assignment. Be sure to explain your reasoning.
I'm guessing one of the reasons why lies in the meaning of POSIX, that we're
going through an operating system (OS) and its files and must abide by the
great wizard overseeing its artifacts as Travis mentioned.  We are using FLAGS
for reading that is centralized over all systems, which is one of the reasons
why we use POSIX instead of C standard library.  Another reason is that
POSIX structures like dirent struct provide useful information on files with its
macros like IS_DIR or if its a regular file -- I can't seem to recall that in
the C standard library when we were using those read() or fget() -- POSIX allows
us to do more with the file/directory in this manner intel-wise.

## Question 4
##### In searchshell.c you had to come up with ways of dealing with user input. What were some things (>= 2) you encountered when handling user input that were challenging? How does user input handling compare to other languages you have programmed in? (>=2 comparisons)
:(  I didn't end up doing searchshell.c but I will list some things I imagine
being difficult with user input.  One difficult thing would be user input
is erratic and unpredictable -- on edstem there was a post on knowing when
the user presses CTRL+C or CTRL+D to end the program.  I remember from past
assignments dealing with user input, that they would input incorrect/invalid
input, so then having to parse the input to see if it's correct or incorrect 
can prove challenging as well since we are dealing with char *.  We can't just
strcomp the whole thing since we're searching for words and we have to check
if what they gave us was a word isalpha.

Compared to other languages, user input seems much more difficult to handle as
we are evaluating char rather than the whole string in our case so it seems
less abstract in a sense as we deal with the individual character rather than
the whole string -- it's kind of like dealing with a char array in java rather
than just string with its handy dandy methods.  Input handling also differs
as we have to control our own garbage disposal of the input the user gave in.

