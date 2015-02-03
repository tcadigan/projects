# Snobol III

original source: http://minnie.tuhs.org/cgi-bin/utree.pl?file=V6/usr/source/sno

Goals:

1) Re-type all source code [Done]

2) Get it to compile [Done]

3) Likely above won't work right, reimplement so it does [WIP]

Notable changes:

1) Strings were not strongly linked, they did not link backwards. They now do.

2) Memory allocation was using sbrk. Re-implemented using malloc. This changes how freespace was handled.