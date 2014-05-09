asyncps
=======

An [asynchronous publish subscribe](http://en.wikipedia.org/wiki/Publish%E2%80%93subscribe_pattern) system on which subscribers dont block
on new messages.


## Building
    $ cd src && make


## Publishing and Subscribing
I didnt write a client yet, so
you should use telnet to publish and subscribe.

Start the server:

    $ bin/asynps 127.0.0.1 8080

Start a telnet session in another terminal:

    $ telnet 127.0.0.1 8080
    publish mongo hello freaks


Start a new telnet session on another terminal:

    $ telnet 127.0.0.1 8080
    subscribe mongo
    hello freaks

## About
asyncps is part of my research on distributed systems, so do not
expect a state--of--the--heart design or good coding practices, 
losely coupled modules. It was written for testing purposes only.
It is an effort to design better distributed systems, mainly
composed by these modules:

    * networking
    * logging
    * event programming
    * efficient FSMs(Finit State Machines)
    * sync/async i/o
    * and others

## Logging
I am one of those freaky guys on logging, so i wrote
a small logger with color capabilities.
Read logging.h, logging.cpp
for more information.
