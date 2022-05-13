1. Convert tdms file into root
The python file for converting is tdms2root.py, but there is another bash file, tdmsloop.sh, is for auto-converting all files in tdms/. Thus the only thing to do is executing tdmsloop.sh to convert all tdms files into root.

2. Re-classify root file in data
Executing autoClaasifying.sh to classify root files in data by its date.

3. Set time range and resolution for analysis
The example for assigning time is document/TIexample1.txt, document/TIexample2.txt and document/TIexample3.txt. This three file explain how long the analysis time is and how big the resolution is.

4. Fill histogram
e.g. ./go -up -if document/repeatTIexample1.txt -of output1
-if document/repeatTIexample1.txt means which time range you assign. It depends on the file generate in step 3.
-of output1 means a new file named output1 will be made in the directory analyzedFile.
The default of new file state is recreate, but if want to renew the output file, add option -up in the command.

5. Do function fitting and counting
e.g. ./go -fit -if output1 -of fitOutput1
e.g. ./go -count -if output1 -of countOutput1
Choose the analysis method, -fit and -count.
-if output1 assigns source file.
-of fitOutput1 means the analysis result will output to this root file in analyzedFile/.

6. Draw graphs
e.g. ./go -g -if fitOutput1
Print the analysis graph. The class for graph drawing is GraphPrinter in include/ and src/. The graph is stored in plotting/.

Options:
-h, --help: help manual
-t, --test: for function testing
-a, --fillHist: fill each data entry into histograms
-fit, --fitting: do function fitting on each histograms
-count, --counting: do counting for each histogram
-g, --printGraph: generate graph by the analyses of fitting or counting
-if, --inputFile: assign input file for analysis
-of, --outputFile: assing the name of file with output analysis data
-re, --recreate: if the output file exists, the new output will be recreated and cover the old one
-up, --update: if the output file exists, the new output will add into the old file.
-TI, --rewriteTI: the txt file for assigning time range will be rewritten. It is for auto-conducting
