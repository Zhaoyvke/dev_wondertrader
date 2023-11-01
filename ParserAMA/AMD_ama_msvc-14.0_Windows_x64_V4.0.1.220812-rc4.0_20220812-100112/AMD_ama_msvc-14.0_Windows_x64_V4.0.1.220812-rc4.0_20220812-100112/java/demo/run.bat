@echo off

javac -encoding utf-8 -classpath ../ama.jar ./*.java -d .
jar cvf demo.jar ./

set PATH=%PATH%;../lib
java -cp .;../lib;../ama.jar;demo.jar demo.ama.AmaDemo

