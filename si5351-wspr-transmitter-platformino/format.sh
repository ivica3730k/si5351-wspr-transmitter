#!/bin/bash

find include src -type f -regex '.*\.\(cpp\|h\)' -exec clang-format -i {} +
