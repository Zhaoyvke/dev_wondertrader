@echo off

set PATH=%PATH%;../lib
bin\ama_test.exe --log-to-console --cfg-name ./etc/ama.json --data-to-console --data-to-csv --snapshot --index-snapshot --tick-order --tick-execution --bond-snapshot --hk-snapshot --order-book 
