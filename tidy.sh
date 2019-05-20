#!/bin/bash

astyle -n --style=linux --pad-comma --pad-oper --delete-empty-lines --unpad-paren src/*.cpp include/*.hpp test/*/*.cpp
