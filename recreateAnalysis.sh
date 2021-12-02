#!/bin/bash
./go -re -if document/0418to0607.txt -of output
./go -fit -if output -of fitOutput
./go -count -if output -of countOutput
