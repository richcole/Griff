#!/bin/bash
grep "Release Version:" version | sed -e 's/Release Version: *//'
