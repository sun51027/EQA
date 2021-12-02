#!/bin/bash
./go -up -if document/afterSummer.txt -of output
./go -fit -if output -of fitOutput
./go -count -if output -of countOutput
