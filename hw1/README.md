# HW1 Short Answer Questions  
With each HW Assignment, there will be a few short answer questions that for you to fill out.  
These questions will hopefully require you to reflect back on your experiences with the assignment.  
The questions can be found on the HW specification on the website. Please write at least a few sentences for each question.

## Question 1
##### To take this class, you must have done some Object-Oriented Programming in Java. Name a few (>=3) things that you noticed were more complex and/or difficult when building data structures in C.
When building data structures, we must be managing memory using mallocs and these puzzles make
building data structures much more difficult.. we have to be the garbage collector unlike Java.
Data structure buildings also involve generics usually, and that is much difficult in C
as we have to recognize the differences in what we are pointing to.. an address?  A pointer?  A value?
Another difficult thing was all the casting and usage of pointers.  It is easy to know
distinctly what type things are in Java I feel at least, 
or their relationship/inheritance/classes, but it gets more messy for me 
(might just be unfamiliar/refresher right now).


## Question 2
##### What are some (>=2)  advantages to developing data structures in C when compared to languages like Java, which have a higher level of abstraction?
Some advantages in C, although it is more difficult, is the fact that we can actually
manage our memory allocation in C when we are building these data structures and thus our speed
of our data structures because some data implementations would be faster than others (big vs small datasets).
Another advantage is that we can allow our data structures to use very fine, detailed, and specific
ways of storing data.  -cough- I remember trying to store a generic type in java with arrays
and you can't do that!  But you could in C.  :)


## Question 3
##### For these data structures to be generic, we used a void\* for the Payloads of such structures. What difficulties did you experience with this implementation of generics? What pitfalls does this implementation of generics have?
Oh my gosh don't even get me started on this.  I still ended up close but not quite
because I have 9 valgrind mallocs.  The main difficulty that I kept messing up on was
how to correctly cast or refer to the value, address, or pointer when I was using the void*.
The * and & really confuse me.  Pitfalls this has is this can make memory leaks easy
if we malloc and don't free.. which happened to me.  You also have to cast properly before
you can do anything with the pointer, its values, arithmetic, dereference, etc.  Yikes.
I struggled.