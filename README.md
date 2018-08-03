# ns-3-http-traffic-generator

This is an implementation of a HTTP Traffic Generator. The implementation of this application is simplistic and it does not support pipelining in this current version.

The model used is based on the distributions indicated in the paper "An HTTP Web Traffic Model Based on the Top One Million Visited Web Pages" by Rastin Pries et. al. This simplistic approach was taken since this traffic generator was developed primarily to help users evaluate their proposed scheduling algorithms in other modules of ns-3 (for example LENA). To allow deeper studies about the HTTP Protocol, it needs some improvements.

It is also important to say that this module was developed to work with ns-3.13 and only with IPv4. However, the code was updated and the last checked version was ns-3.27.

To run this application, you also have to add the HTTP Header to your NS-3 code.

The files were placed where they should be in your ns-3 instalation, i.e., in the directories /src/applications and /src/internet.

The wscript files were included as examples. Search for the http entries in the wscript examples and make the insertions in your wscript files. If you just replace the wscript files you can mess other modules in your ns-3 instalation.
