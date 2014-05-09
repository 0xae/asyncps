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

## Beware
ahmmm, in the good design sense, asyncps is a complete mess, i am
trying to design better programs and feel happy with that,
i do not like its architecture, since i had to design his main
components from scratch:
    * networking module
    * logging module
    * event programming module

## Logging
I am one of those freaky guys on logging, so i wrote
a small logger with color capabilities.
Read logging.h, logging.cpp
for more information.
