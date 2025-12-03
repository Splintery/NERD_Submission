# Overview/Main idea

When I first started to think about a solution I wanted to avoid any major bottlenecks.
That is why each thread has its own MailBox object where it pulls messages sent by other threads.
My goal zas to keep as much work done in parallel as possible, so as long as 2 distinct threads do not try to acces one specific queue there is no need to block execution. I chose to use the [deque](https://en.cppreference.com/w/cpp/container/deque.html) structure to store the pending messages. Random acces to an element and insertion/removal at the front/back is done in O(1). I chose to add new messages at the back and to read them from the front (behaves like a queue).

## Program structure
When tackling with thread synchronisation I am used to drawing on paper a [petri net](https://fr.wikipedia.org/wiki/R%C3%A9seau_de_Petri). It allows to isolate the differents task a thread has into states:
* Redirecting
* Accepting
* Waiting
* Finished

The role of the Petri net is to make it clear how a thread can go from one state to another. I uploaded my drawing as petri_net_REC2659.jpg.

## Big problem and my struggles
The harder part of this exercise was figuring out when should one thread stop.
Because I use condition variables so the waiting threads don't consume processing power I need a way to wake all them so they can exit the function properly when the work is done

# I went about this problem in 3 different ways:
*1.* I figured I could keep an atomic variable to track how many of my threads are waiting/sleeping, when incrementing, if the current thread going to sleep is the last one, it is its responsability to wake up every other thread to exit the function
This approach failed because a thread could see itself as the last one in the timeframe between another thread waking up and decrementing the counter calling for exit when there were still messages to be exchanged.

*2.* I saw the problem from the opposite angle and used a counter to track the pending messages (a message sent but not yet received).
With this approach, before going to sleep if there are no messages pending the thread going to sleep would call for exit.
This time again a thread could see this happening prematurely because it doesn't account for threads that are about to send a message (i.e. not sleeping)

*3.* Last approach is a fusion of the 2 previous ones. I use 2 counters, one to track the sleeping threads and the other to track the flying messages.
By doing so I was sure that when a thread saw itself as the last thread to go to sleep, there would be no message pending that would otherwise wake up another thread.
