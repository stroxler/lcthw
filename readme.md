# Learn C the Hard Way, by Zed Shaw and coded by Steven Troxler

Zed Shaw's ebook *Learn C the Hard Way* is an amazing resource for learning
C, and I'm very grateful to him for creating it.

I've been working through the code on terminal.com, and as thorough as Zed
has been, there have still been some issues for me using his code. In
particular, the makefiles he uses for the more advanced code don't reliably
work for me.

Because of that, I'm creating this repository to share the changes I made
in order to make the code work and compile in an ubuntu enviroment, and
also to record the considerable notes and comments I've made for my own sake
along the way.

I encourage any readers to take my comments and notes with a major grain of
salt; there are probably plenty of mistakes or misunderstandings.

Note that you'll need some setup in order to compile C code on a fresh ubuntu
install. Run
`sudo apt-get install -y build-essential`
which should get you gcc and some other basic tools. You may need a separate
install of valgring, also easy to obtain from `apt`, later on.

## Organization

I broke the early code up into parts based on difficulty level. You can
see notes on what is in each part in part_notes.

A rough breakdown of the content of different exercises can be seen in
ex_notes.

I added notes on library functions introduced in lib_notes, but past a certain
point -- I think exercise 25 or so -- it is very incomplete.

Be aware that the c-skeleton makefile may be out of date, and may not work
properly on ubuntu. For a version of the skeleton which is functionally
certain, look at ex30 or at liblcthw, which are both functional for their
respective projects.
