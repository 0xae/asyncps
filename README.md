asyncps
=======

Asynchronous Publish Subscribe
An asynchronous publish subscribe system on which subscribers dont block
on new messages.

building
=========
$ cd src && make


publishing and subscribing
===================
I didnt write a client yet, so
you should use telnet to publish and subscribe

Start the server in a terminal
$ bin/asynpc <host> <port>

Start a telnet session in another terminal
$ telnet <host> <port>
