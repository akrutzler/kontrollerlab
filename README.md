KontrollerLab
=============

The KontrollerLab is an IDE for developing software for Atmel(r) AVR(c) microcontrollers using the avr-gcc compiler, the uisp and the avrdude upload software.

Introduction
-------------

Resources
-------------
- KontrollerLab on github:
    https://github.com/andik92/kontrollerlab

- KontrollerLab on ArchLinux AUR
    https://aur.archlinux.org/packages/kontrollerlab-git/

- source code repositories: see Getting the source section, below

Getting the source
------------------

KontrollerLab's source code is located in GIT repositories.

The official repository is located on github:

- https://github.com/andik92/kontrollerlab

- the master branch contains the latest tested development code,
    while other branches might contain untested/unstable code

Checking out from GIT can be performed with the following commands:

    git clone git://github.com/andik92/kontrollerlab
    cd kontrollerlab
    git checkout -b master --track origin/master
    
Building and Running
--------------------

Required dependencies

- KDE4 libraries -dev
- Qt4 -dev
- cmake
- automoc4
- glib -dev

1. By using a terminal, navigate to the top-level source-directory

2. Create the makefiles using cmake

         cmake .

3. Compile the project

         make
         
3. Install the project

         make install
         
5. Run KontrollerLab

        kontrollerlab
        
KontrollerLab should start running at this point.

