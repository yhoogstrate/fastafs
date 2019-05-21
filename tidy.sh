#!/bin/bash

astyle -n --style=linux --pad-comma --pad-oper --unpad-paren src/*.cpp include/*.hpp test/*/*.cpp
