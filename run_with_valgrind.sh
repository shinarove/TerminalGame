#!/usr/bin/env bash

valgrind --leak-check=full --log-file=log/valgrind_log%p.txt ./buildDir/TerminalGame