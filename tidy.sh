#!/bin/bash

astyle -n --style=linux --add-braces --pad-comma --pad-oper --unpad-paren src/*.cpp include/*.hpp test/*/*.cpp
