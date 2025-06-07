#!/usr/bin/env bash

valgrind --leak-check=full --tool=memcheck --log-file=log/valgrind_log%p.txt ./buildDir/TerminalGame