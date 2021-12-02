#!/bin/bash
./go -up -if document/0607to0716.txt -of output
./go -fit -if output -of fitOutput
./go -count -if output -of countOutput
