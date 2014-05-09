asyncps
=======
Asynchronous Publish Subscribe
An asynchronous publish subscribe system on which subscribers dont block
on new messages.


## building
    $ cd src && make


## publishing and subscribing
I didnt write a client yet, so
you should use telnet to publish and subscribe.

Start the server in a terminal:

    $ bin/asynpc 127.0.0.1 8080

Start a telnet session in another terminal:

    $ telnet 127.0.0.1 8080
    Trying 127.0.0.1...
    Connected to 127.0.0.1.
    Escape character is '^]'.
    publish mongo hello freaks


Start a new telnet session on another terminal:

    $ telnet 127.0.0.1 8080
    Trying 127.0.0.1...
    Connected to 127.0.0.1.
    Escape character is '^]'.
    subscribe mongo
    hello freaks
