#!/bin/bash

astyle -n --style=linux --pad-comma --pad-oper --delete-empty-lines src/*.cpp include/*.hpp test/*/*.cpp
